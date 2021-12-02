#include <cstring>
#include <algorithm>
#include <cmath>

#include "hub75.hpp"


// Basic function to convert Hue, Saturation and Value to an RGB colour
Pixel hsv_to_rgb(float h, float s, float v) {
    if(h < 0.0f) {
        h = 1.0f + fmod(h, 1.0f);
    }

    int i = int(h * 6);
    float f = h * 6 - i;

    v = v * 255.0f;

    float sv = s * v;
    float fsv = f * sv;

    auto p = uint8_t(-sv + v);
    auto q = uint8_t(-fsv + v);
    auto t = uint8_t(fsv - sv + v);

    uint8_t bv = uint8_t(v);

    switch (i % 6) {
        default:
        case 0: return Pixel(bv, t, p);
        case 1: return Pixel(q, bv, p);
        case 2: return Pixel(p, bv, t);
        case 3: return Pixel(p, q, bv);
        case 4: return Pixel(t, p, bv);
        case 5: return Pixel(bv, p, q);
    }
}

Hub75::Hub75(uint width, uint height, Pixel *buffer, PanelType panel_type, bool inverted_stb)
 : width(width), height(height), panel_type(panel_type), inverted_stb(inverted_stb)
 {
    // Set up allllll the GPIO
    gpio_init(pin_r0); gpio_set_function(pin_r0, GPIO_FUNC_SIO); gpio_set_dir(pin_r0, true); gpio_put(pin_r0, 0);
    gpio_init(pin_g0); gpio_set_function(pin_g0, GPIO_FUNC_SIO); gpio_set_dir(pin_g0, true); gpio_put(pin_g0, 0);
    gpio_init(pin_b0); gpio_set_function(pin_b0, GPIO_FUNC_SIO); gpio_set_dir(pin_b0, true); gpio_put(pin_b0, 0);

    gpio_init(pin_r1); gpio_set_function(pin_r1, GPIO_FUNC_SIO); gpio_set_dir(pin_r1, true); gpio_put(pin_r1, 0);
    gpio_init(pin_g1); gpio_set_function(pin_g1, GPIO_FUNC_SIO); gpio_set_dir(pin_g1, true); gpio_put(pin_g1, 0);
    gpio_init(pin_b1); gpio_set_function(pin_b1, GPIO_FUNC_SIO); gpio_set_dir(pin_b1, true); gpio_put(pin_b1, 0);

    gpio_init(pin_row_a); gpio_set_function(pin_row_a, GPIO_FUNC_SIO); gpio_set_dir(pin_row_a, true); gpio_put(pin_row_a, 0);
    gpio_init(pin_row_b); gpio_set_function(pin_row_b, GPIO_FUNC_SIO); gpio_set_dir(pin_row_b, true); gpio_put(pin_row_b, 0);
    gpio_init(pin_row_c); gpio_set_function(pin_row_c, GPIO_FUNC_SIO); gpio_set_dir(pin_row_c, true); gpio_put(pin_row_c, 0);
    gpio_init(pin_row_d); gpio_set_function(pin_row_d, GPIO_FUNC_SIO); gpio_set_dir(pin_row_d, true); gpio_put(pin_row_d, 0);
    gpio_init(pin_row_e); gpio_set_function(pin_row_e, GPIO_FUNC_SIO); gpio_set_dir(pin_row_e, true); gpio_put(pin_row_e, 0);

    gpio_init(pin_clk); gpio_set_function(pin_clk, GPIO_FUNC_SIO); gpio_set_dir(pin_clk, true); gpio_put(pin_clk, !clk_polarity);
    gpio_init(pin_stb); gpio_set_function(pin_stb, GPIO_FUNC_SIO); gpio_set_dir(pin_stb, true); gpio_put(pin_clk, !stb_polarity);
    gpio_init(pin_oe); gpio_set_function(pin_oe, GPIO_FUNC_SIO); gpio_set_dir(pin_oe, true); gpio_put(pin_clk, !oe_polarity);

    if (buffer == nullptr) {
        front_buffer = new Pixel[width * height];
        back_buffer = new Pixel[width * height];
        managed_buffer = true;
    } else {
        front_buffer = buffer;
        back_buffer = buffer + width * height;
        managed_buffer = false;
    }

    if (brightness == 0) {
        if (width >= 64) brightness = 6;
        if (width >= 96) brightness = 3;
        if (width >= 128) brightness = 2;
        if (width >= 160) brightness = 1;
    }
}

void Hub75::set_color(uint x, uint y, Pixel c) {
    int offset = 0;
    if(x >= width || y >= height) return;
    if(y >= height / 2) {
        y -= height / 2;
        offset = (y * width + x) * 2;
        offset += 1;
    } else {
        offset = (y * width + x) * 2;
    }
    front_buffer[offset] = c;
}

void Hub75::set_rgb(uint x, uint y, uint8_t r, uint8_t g, uint8_t b) {
    set_color(x, y, Pixel(r, g, b));
}

void Hub75::set_hsv(uint x, uint y, float h, float s, float v) {
    set_color(x, y, hsv_to_rgb(h, s, v));
}

void Hub75::FM6126A_write_register(uint16_t value, uint8_t position) {
    gpio_put(pin_clk, !clk_polarity);
    gpio_put(pin_stb, !stb_polarity);

    uint8_t threshold = width - position;
    for(auto i = 0u; i < width; i++) {
        auto j = i % 16;
        bool b = value & (1 << j);

        gpio_put(pin_r0, b);
        gpio_put(pin_g0, b);
        gpio_put(pin_b0, b);
        gpio_put(pin_r1, b);
        gpio_put(pin_g1, b);
        gpio_put(pin_b1, b);

        // Assert strobe/latch if i > threshold
        // This somehow indicates to the FM6126A which register we want to write :|
        gpio_put(pin_stb, i > threshold);
        gpio_put(pin_clk, clk_polarity);
        sleep_us(10);
        gpio_put(pin_clk, !clk_polarity);
    }
}

void Hub75::FM6126A_setup() {
    // Ridiculous register write nonsense for the FM6126A-based 64x64 matrix
    FM6126A_write_register(0b1111111111111110, 12);
    FM6126A_write_register(0b0000001000000000, 13);
}

void Hub75::start(irq_handler_t handler) {
    if(handler) {
        dma_channel = 0;

        // Try as I might, I can't seem to coax MicroPython into leaving PIO in a known state upon soft reset
        // check for claimed PIO and prepare a clean slate.
        stop(handler);

        if (panel_type == PANEL_FM6126A) {
            FM6126A_setup();
        }

        // Claim the PIO so we can clean it upon soft restart
        pio_sm_claim(pio, sm_data);
        pio_sm_claim(pio, sm_row);

        data_prog_offs = pio_add_program(pio, &hub75_data_rgb888_program);
        if (inverted_stb) {
            row_prog_offs = pio_add_program(pio, &hub75_row_inverted_program);
        } else {
            row_prog_offs = pio_add_program(pio, &hub75_row_program);
        }
        hub75_data_rgb888_program_init(pio, sm_data, data_prog_offs, DATA_BASE_PIN, pin_clk);
        hub75_row_program_init(pio, sm_row, row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, pin_stb);

        // Prevent flicker in Python caused by the smaller dataset just blasting through the PIO too quickly
        pio_sm_set_clkdiv(pio, sm_data, width <= 32 ? 2.0f : 1.0f);

        dma_channel_claim(dma_channel);
        dma_channel_config config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
        channel_config_set_bswap(&config, false);
        channel_config_set_dreq(&config, pio_get_dreq(pio, sm_data, true));
        dma_channel_configure(dma_channel, &config, &pio->txf[sm_data], NULL, 0, false);

        dma_channel_claim(dma_flip_channel);
        dma_channel_config flip_config = dma_channel_get_default_config(dma_flip_channel);
        channel_config_set_transfer_data_size(&flip_config, DMA_SIZE_32);
        channel_config_set_read_increment(&flip_config, true);
        channel_config_set_write_increment(&flip_config, true);
        channel_config_set_bswap(&flip_config, false);
        dma_channel_configure(dma_flip_channel, &flip_config, nullptr, nullptr, 0, false);

        // Same handler for both DMA channels
        irq_set_exclusive_handler(DMA_IRQ_0, handler);
        irq_set_exclusive_handler(DMA_IRQ_1, handler);

        dma_channel_set_irq1_enabled(dma_flip_channel, true);
        dma_channel_set_irq0_enabled(dma_channel, true);

        irq_set_enabled(pio_get_dreq(pio, sm_data, true), true);
        irq_set_enabled(DMA_IRQ_0, true);
        irq_set_enabled(DMA_IRQ_1, true);

        row = 0;
        bit = 0;

        hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
        dma_channel_set_trans_count(dma_channel, width * 2, false);
        dma_channel_set_read_addr(dma_channel, &back_buffer, true);
    }
}

void Hub75::stop(irq_handler_t handler) {
    do_flip = false;

    irq_set_enabled(DMA_IRQ_0, false);
    irq_set_enabled(DMA_IRQ_1, false);
    irq_set_enabled(pio_get_dreq(pio, sm_data, true), false);

    if(dma_channel_is_claimed(dma_channel)) {
        dma_channel_set_irq0_enabled(dma_channel, false);
        irq_remove_handler(DMA_IRQ_0, handler);
        //dma_channel_wait_for_finish_blocking(dma_channel);
        dma_channel_abort(dma_channel);
        dma_channel_acknowledge_irq0(dma_channel);
        dma_channel_unclaim(dma_channel);
    }

    if(dma_channel_is_claimed(dma_flip_channel)){
        dma_channel_set_irq1_enabled(dma_flip_channel, false);
        irq_remove_handler(DMA_IRQ_1, handler);
        //dma_channel_wait_for_finish_blocking(dma_flip_channel);
        dma_channel_abort(dma_flip_channel);
        dma_channel_acknowledge_irq1(dma_flip_channel);
        dma_channel_unclaim(dma_flip_channel);
    }

    if(pio_sm_is_claimed(pio, sm_data)) {
        pio_sm_set_enabled(pio, sm_data, false);
        pio_sm_drain_tx_fifo(pio, sm_data);
        pio_sm_unclaim(pio, sm_data);
    }

    if(pio_sm_is_claimed(pio, sm_row)) {
        pio_sm_set_enabled(pio, sm_row, false);
        pio_sm_drain_tx_fifo(pio, sm_row);
        pio_sm_unclaim(pio, sm_row);
    }

    pio_clear_instruction_memory(pio);

    // Make sure the GPIO is in a known good state
    // since we don't know what the PIO might have done with it
    gpio_put_masked(0b111111 << pin_r0, 0);
    gpio_put_masked(0b11111 << pin_row_a, 0);
    gpio_put(pin_clk, !clk_polarity);
    gpio_put(pin_clk, !oe_polarity);
}

Hub75::~Hub75() {
    if (managed_buffer) {
        delete[] front_buffer;
        delete[] back_buffer;
    }
}

void Hub75::clear() {
    for(auto x = 0u; x < width; x++) {
        for(auto y = 0u; y < height; y++) {
            set_rgb(x, y, 0, 0, 0);
        }
    }
}

void Hub75::flip(bool copybuffer) {
    dma_channel_config flip_config = dma_get_channel_config(dma_flip_channel);
    channel_config_set_read_increment(&flip_config, copybuffer);
    dma_channel_configure(dma_flip_channel, &flip_config, nullptr, nullptr, 0, false);

    dma_channel_set_read_addr(dma_flip_channel, copybuffer ? front_buffer : &background, false);
    dma_channel_set_write_addr(dma_flip_channel, back_buffer, false);

    // Flip and block until the front buffer has been prepared
    do_flip = true;
    while(do_flip) {
        best_effort_wfe_or_timeout(make_timeout_time_us(10));
    };
}

void Hub75::dma_complete() {
    if(dma_channel_get_irq1_status(dma_flip_channel)) {
        dma_channel_acknowledge_irq1(dma_flip_channel);
        do_flip = false;
    }

    if(dma_channel_get_irq0_status(dma_channel)) {
        dma_channel_acknowledge_irq0(dma_channel);

        // Push out a dummy pixel for each row
        pio_sm_put_blocking(pio, sm_data, 0);
        pio_sm_put_blocking(pio, sm_data, 0);

        // SM is finished when it stalls on empty TX FIFO
        hub75_wait_tx_stall(pio, sm_data);

        // Check that previous OEn pulse is finished, else things WILL get out of sequence
        hub75_wait_tx_stall(pio, sm_row);

        // Latch row data, pulse output enable for new row.
        pio_sm_put_blocking(pio, sm_row, row | (brightness << 5 << bit));

        if (do_flip && bit == 0 && row == 0) {
            // Literally flip the front and back buffers by swapping their addresses
            Pixel *tmp = back_buffer;
            back_buffer = front_buffer;
            front_buffer = tmp;
            // Then, read the contents of the back buffer into the front buffer
            dma_channel_set_trans_count(dma_flip_channel, width * height, true);
        }

        row++;

        if(row == height / 2) {
            row = 0;
            bit++;
            if (bit == BIT_DEPTH) {
                bit = 0;
            }
            hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
        }

        dma_channel_set_trans_count(dma_channel, width * 2, false);
        dma_channel_set_read_addr(dma_channel, &back_buffer[row * width * 2], true);
    }
}