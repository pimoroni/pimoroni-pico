#include <cstring>
#include <algorithm>
#include <cmath>

#include "hardware/clocks.h"

#include "duo75.hpp"

namespace pimoroni {

#define GPIO_INIT_BOTH(pin, initial) gpio_init(pin); gpio_set_function(pin, GPIO_FUNC_SIO); gpio_set_dir(pin, true); gpio_put(pin, initial); \
                                     gpio_init(pin + 32); gpio_set_function(pin + 32, GPIO_FUNC_SIO); gpio_set_dir(pin + 32, true); gpio_put(pin + 32, initial)

Duo75::Duo75(Pixel *buffer, PanelType panel_type, bool inverted_stb, COLOR_ORDER color_order)
 : panel_type(panel_type), inverted_stb(inverted_stb), color_order(color_order)
 {
    // Set up allllll the GPIO
    GPIO_INIT_BOTH(pin_r0, 0);
    GPIO_INIT_BOTH(pin_g0, 0);
    GPIO_INIT_BOTH(pin_b0, 0);

    GPIO_INIT_BOTH(pin_r1, 0);
    GPIO_INIT_BOTH(pin_g1, 0);
    GPIO_INIT_BOTH(pin_b1, 0);

    GPIO_INIT_BOTH(pin_row_a, 0);
    GPIO_INIT_BOTH(pin_row_b, 0);
    GPIO_INIT_BOTH(pin_row_c, 0);
    GPIO_INIT_BOTH(pin_row_d, 0);
    GPIO_INIT_BOTH(pin_row_e, 0);

    GPIO_INIT_BOTH(pin_clk, !clk_polarity);
    GPIO_INIT_BOTH(pin_stb, !stb_polarity);
    GPIO_INIT_BOTH(pin_oe, !oe_polarity);

    if (buffer == nullptr) {
        back_buffer = new Pixel[width * height];
        managed_buffer = true;
    } else {
        back_buffer = buffer;
        managed_buffer = false;
    }

    if (brightness == 0) {
#if PICO_RP2350
        brightness = 4;
#else
        if (width >= 64) brightness = 6;
        if (width >= 96) brightness = 3;
        if (width >= 128) brightness = 2;
        if (width >= 160) brightness = 1;
#endif
    }

    switch (color_order) {
        case COLOR_ORDER::RGB:
            r_shift = 0;
            g_shift = 10;
            b_shift = 20;
            break;
        case COLOR_ORDER::RBG:
            r_shift = 0;
            g_shift = 20;
            b_shift = 10;
            break;
        case COLOR_ORDER::GRB:
            r_shift = 20;
            g_shift = 0;
            b_shift = 10;
            break;
        case COLOR_ORDER::GBR:
            r_shift = 10;
            g_shift = 20;
            b_shift = 0;
            break;
        case COLOR_ORDER::BRG:
            r_shift = 10;
            g_shift = 00;
            b_shift = 20;
            break;
        case COLOR_ORDER::BGR:
            r_shift = 20;
            g_shift = 10;
            b_shift = 0;
            break;

    }
}

void Duo75::set_color(uint x, uint y, Pixel c) {
    Pixel *buf = back_buffer + width * height / 2;
    int offset = 0;
    if(x >= width || y >= height) return;

    if(y >= height / 2) {
        y -= height / 2;
        buf = back_buffer;
    }

    if(y >= height / 4) {
        y -= height / 4;
        offset = (y * width + x) * 2;
        offset += 1;
    } else {
        offset = (y * width + x) * 2;
    }
    buf[offset] = c;
}

void Duo75::set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b) {
    Pixel *buf = back_buffer + width * height / 2;
    int offset = 0;
    if(x >= width || y >= height) return;

    if(y >= height / 2) {
        y -= height / 2;
        buf = back_buffer;
    }

    if(y >= height / 4) {
        y -= height / 4;
        offset = (y * width + x) * 2;
        offset += 1;
    } else {
        offset = (y * width + x) * 2;
    }
    buf[offset] = (GAMMA_10BIT[b] << b_shift) | (GAMMA_10BIT[g] << g_shift) | (GAMMA_10BIT[r] << r_shift);
}

void gpio_put_both(uint pin, bool value) {
    gpio_put(pin, value);
    gpio_put(pin + 32, value);
}

void Duo75::FM6126A_write_register(uint16_t value, uint8_t position) {
    gpio_put_both(pin_clk, !clk_polarity);
    gpio_put_both(pin_stb, !stb_polarity);

    uint8_t threshold = width - position;
    for(auto i = 0u; i < width; i++) {
        auto j = i % 16;
        bool b = value & (1 << j);

        gpio_put_both(pin_r0, b);
        gpio_put_both(pin_g0, b);
        gpio_put_both(pin_b0, b);
        gpio_put_both(pin_r1, b);
        gpio_put_both(pin_g1, b);
        gpio_put_both(pin_b1, b);

        // Assert strobe/latch if i > threshold
        // This somehow indicates to the FM6126A which register we want to write :|
        gpio_put_both(pin_stb, i > threshold);
        gpio_put_both(pin_clk, clk_polarity);
        sleep_us(10);
        gpio_put_both(pin_clk, !clk_polarity);
    }
}

void Duo75::FM6126A_setup() {
    // Ridiculous register write nonsense for the FM6126A-based 64x64 matrix
    FM6126A_write_register(0b1111111111111110, 12);
    FM6126A_write_register(0b0000001000000000, 13);
}

void Duo75::start(irq_handler_t handler) {
    if(handler) {
        if (panel_type == PANEL_FM6126A) {
            FM6126A_setup();
        }

        uint latch_cycles = clock_get_hz(clk_sys) / 4000000;

        // Claim the PIO so we can clean it upon soft restart
        pio_sm_claim(pio_a, sm_data_a);
        pio_sm_claim(pio_a, sm_row_a);
        pio_sm_claim(pio_b, sm_data_b);
        pio_sm_claim(pio_b, sm_row_b);

        pio_set_gpio_base(pio_b, 16);

        data_prog_offs_a = pio_add_program(pio_a, &duo75_data_rgb888_program);
        data_prog_offs_b = pio_add_program(pio_b, &duo75_data_rgb888_program);
        if (inverted_stb) {
            row_prog_offs_a = pio_add_program(pio_a, &duo75_row_inverted_program);
            row_prog_offs_b = pio_add_program(pio_b, &duo75_row_inverted_program);
        } else {
            row_prog_offs_a = pio_add_program(pio_a, &duo75_row_program);
            row_prog_offs_b = pio_add_program(pio_b, &duo75_row_program);
        }
        duo75_data_rgb888_program_init(pio_a, sm_data_a, data_prog_offs_a, DATA_BASE_PIN, pin_clk);
        duo75_data_rgb888_program_init(pio_b, sm_data_b, data_prog_offs_b, DATA_BASE_PIN + 32, pin_clk + 32);

        duo75_row_program_init(pio_a, sm_row_a, row_prog_offs_a, ROWSEL_BASE_PIN, ROWSEL_N_PINS, pin_stb, latch_cycles);
        duo75_row_program_init(pio_b, sm_row_b, row_prog_offs_b, ROWSEL_BASE_PIN + 32, ROWSEL_N_PINS, pin_stb + 32, latch_cycles);

        // Prevent flicker in Python caused by the smaller dataset just blasting through the PIO too quickly
        pio_sm_set_clkdiv(pio_a, sm_data_a, width <= 32 ? 2.0f : 1.0f);
        pio_sm_set_clkdiv(pio_b, sm_data_b, width <= 32 ? 2.0f : 1.0f);

        dma_channel_a = dma_claim_unused_channel(true);
        dma_channel_config config_a = dma_channel_get_default_config(dma_channel_a);
        channel_config_set_transfer_data_size(&config_a, DMA_SIZE_32);
        channel_config_set_bswap(&config_a, false);
        channel_config_set_dreq(&config_a, pio_get_dreq(pio_a, sm_data_a, true));
        dma_channel_configure(dma_channel_a, &config_a, &pio_a->txf[sm_data_a], NULL, 0, false);

        dma_channel_b = dma_claim_unused_channel(true);
        dma_channel_config config_b = dma_channel_get_default_config(dma_channel_b);
        channel_config_set_transfer_data_size(&config_b, DMA_SIZE_32);
        channel_config_set_bswap(&config_b, false);
        channel_config_set_dreq(&config_b, pio_get_dreq(pio_b, sm_data_b, true));
        dma_channel_configure(dma_channel_b, &config_b, &pio_b->txf[sm_data_b], NULL, 0, false);

        // Same handler for both DMA channels
        irq_add_shared_handler(DMA_IRQ_0, handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);

        dma_channel_set_irq0_enabled(dma_channel_a, true);
        dma_channel_set_irq0_enabled(dma_channel_b, true);

        irq_set_enabled(DMA_IRQ_0, true);

        row_a = 0;
        bit_a = 0;
        row_b = 0;
        bit_b = 0;

        duo75_data_rgb888_set_shift(pio_a, sm_data_a, data_prog_offs_a, bit_a);
        duo75_data_rgb888_set_shift(pio_b, sm_data_b, data_prog_offs_b, bit_b);

        dma_channel_set_trans_count(dma_channel_a, width * 2, false);
        dma_channel_set_trans_count(dma_channel_b, width * 2, false);

        dma_channel_set_read_addr(dma_channel_a, &back_buffer, true);
        dma_channel_set_read_addr(dma_channel_b, &back_buffer + (width * height / 2), true);
    }
}

void Duo75::stop(irq_handler_t handler) {

    irq_set_enabled(DMA_IRQ_0, false);

    if(dma_channel_a != -1 &&  dma_channel_is_claimed(dma_channel_a)) {
        dma_channel_set_irq0_enabled(dma_channel_a, false);
        //dma_channel_wait_for_finish_blocking(dma_channel);
        dma_channel_abort(dma_channel_a);
        dma_channel_acknowledge_irq0(dma_channel_a);
        dma_channel_unclaim(dma_channel_a);
    }

    if(dma_channel_b != -1 &&  dma_channel_is_claimed(dma_channel_b)) {
        dma_channel_set_irq0_enabled(dma_channel_b, false);
        //dma_channel_wait_for_finish_blocking(dma_channel);
        dma_channel_abort(dma_channel_b);
        dma_channel_acknowledge_irq0(dma_channel_b);
        dma_channel_unclaim(dma_channel_b);
    }

    irq_remove_handler(DMA_IRQ_0, handler);

    if(pio_sm_is_claimed(pio_a, sm_data_a)) {
        pio_sm_set_enabled(pio_a, sm_data_a, false);
        pio_sm_drain_tx_fifo(pio_a, sm_data_a);
        pio_remove_program(pio_a, &duo75_data_rgb888_program, data_prog_offs_a);
        pio_sm_unclaim(pio_a, sm_data_a);
    }

    if(pio_sm_is_claimed(pio_a, sm_row_a)) {
        pio_sm_set_enabled(pio_a, sm_row_a, false);
        pio_sm_drain_tx_fifo(pio_a, sm_row_a);
        if (inverted_stb) {
            pio_remove_program(pio_a, &duo75_row_inverted_program, row_prog_offs_a);
        } else {
            pio_remove_program(pio_a, &duo75_row_program, row_prog_offs_a);
        }
        pio_sm_unclaim(pio_a, sm_row_a);
    }

    if(pio_sm_is_claimed(pio_b, sm_data_b)) {
        pio_sm_set_enabled(pio_b, sm_data_b, false);
        pio_sm_drain_tx_fifo(pio_b, sm_data_b);
        pio_remove_program(pio_b, &duo75_data_rgb888_program, data_prog_offs_b);
        pio_sm_unclaim(pio_b, sm_data_b);
    }

    if(pio_sm_is_claimed(pio_b, sm_row_b)) {
        pio_sm_set_enabled(pio_b, sm_row_b, false);
        pio_sm_drain_tx_fifo(pio_b, sm_row_b);
        if (inverted_stb) {
            pio_remove_program(pio_b, &duo75_row_inverted_program, row_prog_offs_b);
        } else {
            pio_remove_program(pio_b, &duo75_row_program, row_prog_offs_b);
        }
        pio_sm_unclaim(pio_b, sm_row_b);
    }

    // Make sure the GPIO is in a known good state
    // since we don't know what the PIO might have done with it
    gpio_put_masked(0b111111 << pin_r0, 0);
    gpio_put_masked(0b11111 << pin_row_a, 0);
    gpio_put(pin_clk, !clk_polarity);
    gpio_put(pin_clk, !oe_polarity);

    gpio_put_masked64((uint64_t)0b111111 << (pin_r0 + 32), 0);
    gpio_put_masked64((uint64_t)0b11111 << (pin_row_a + 32), 0);
    gpio_put((pin_clk + 32), !clk_polarity);
    gpio_put((pin_clk + 32), !oe_polarity);
}

Duo75::~Duo75() {
    if (managed_buffer) {
        delete[] back_buffer;
    }
}

void Duo75::clear() {
    for(auto x = 0u; x < width; x++) {
        for(auto y = 0u; y < height; y++) {
            set_pixel(x, y, 0, 0, 0);
        }
    }
}


void Duo75::dma_complete() {

    if(dma_channel_get_irq0_status(dma_channel_a)) {
        dma_channel_acknowledge_irq0(dma_channel_a);

        // Push out a dummy pixel for each row
        pio_sm_put_blocking(pio_a, sm_data_a, 0);
        pio_sm_put_blocking(pio_a, sm_data_a, 0);

        // SM is finished when it stalls on empty TX FIFO
        duo75_wait_tx_stall(pio_a, sm_data_a);

        // Check that previous OEn pulse is finished, else things WILL get out of sequence
        duo75_wait_tx_stall(pio_a, sm_row_a);

        // Latch row data, pulse output enable for new row.
        pio_sm_put_blocking(pio_a, sm_row_a, row_a | (brightness << 5 << bit_a));

        row_a++;

        if(row_a == height / 4) {
            row_a = 0;
            bit_a++;
            if (bit_a == BIT_DEPTH) {
                bit_a = 0;
            }
            duo75_data_rgb888_set_shift(pio_a, sm_data_a, data_prog_offs_a, bit_a);
        }

        dma_channel_set_trans_count(dma_channel_a, width * 2, false);
        dma_channel_set_read_addr(dma_channel_a, &back_buffer[row_a * width * 2], true);
    }

    if(dma_channel_get_irq0_status(dma_channel_b)) {
        dma_channel_acknowledge_irq0(dma_channel_b);

        // Push out a dummy pixel for each row
        pio_sm_put_blocking(pio_b, sm_data_b, 0);
        pio_sm_put_blocking(pio_b, sm_data_b, 0);

        // SM is finished when it stalls on empty TX FIFO
        duo75_wait_tx_stall(pio_b, sm_data_b);

        // Check that previous OEn pulse is finished, else things WILL get out of sequence
        duo75_wait_tx_stall(pio_b, sm_row_b);

        // Latch row data, pulse output enable for new row.
        pio_sm_put_blocking(pio_b, sm_row_b, row_b | (brightness << 5 << bit_b));

        row_b++;

        if(row_b == height / 4) {
            row_b = 0;
            bit_b++;
            if (bit_b == BIT_DEPTH) {
                bit_b = 0;
            }
            duo75_data_rgb888_set_shift(pio_b, sm_data_b, data_prog_offs_b, bit_b);
        }

        dma_channel_set_trans_count(dma_channel_b, width * 2, false);
        dma_channel_set_read_addr(dma_channel_b, &back_buffer[(width * height / 2) + row_b * width * 2], true);
    }
}

void Duo75::copy_to_back_buffer(void *data, size_t len, int start_x, int start_y) {
    uint8_t *p = (uint8_t *)data;

    for(uint y = start_y; y < height; y++) {
        for(uint x = start_x; x < width; x++) {
            // x and y are swapped to achieve a 90degree rotation
            uint sy = x;
            uint sx = width - 1 - y;
            uint offset = sx * 2; // * 2 because we have pairs of pixels

            uint8_t b = *p++;
            uint8_t g = *p++;
            uint8_t r = *p++;
            p++; // Skip empty byte in out 32-bit aligned 24-bit colour.

            // Interlace the top and bottom halves of the panel.
            // Since these are scanned out simultaneously to two chains
            // of shift registers we need each pair of rows
            // (N and N + height / 2) to be adjacent in the buffer.

            // If we're on the second panel, shift up our pixel and adjust
            // the offset to place it into the latter half of the buffer.
            if(sy >= uint(height / 2)) {
                sy -= height / 2;
            } else {
                offset += (width * height / 2);
            }

            // If we're now on the second half of a panel, shift up our
            // pixel and adjust the offset to interlace it.
            if(sy >= uint(height / 4)) {
                sy -= height / 4;
                offset += 1;
            }

            offset += sy * (width * 2);

            back_buffer[offset] = (GAMMA_10BIT[b] << b_shift) | (GAMMA_10BIT[g] << g_shift) | (GAMMA_10BIT[r] << r_shift);

            len -= 4;

            if(len == 0) {
                return;
            }
        }
    }
}

void Duo75::update(PicoGraphics *graphics) {
    if(graphics->pen_type == PicoGraphics::PEN_RGB888) {
        copy_to_back_buffer(graphics->frame_buffer, width * height * sizeof(RGB888), 0, 0);
    } else {
        unsigned int offset = 0;
        graphics->frame_convert(PicoGraphics::PEN_RGB888, [this, &offset, &graphics](void *data, size_t length) {
            if (length > 0) {
                int offset_y = offset / graphics->bounds.w;
                int offset_x = offset - (offset_y * graphics->bounds.w);
                copy_to_back_buffer(data, length, offset_x, offset_y);
                offset += length / sizeof(RGB888);
            }
        });
    }
}
}
