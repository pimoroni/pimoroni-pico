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


Hub75::Hub75(uint8_t width, uint8_t height, Pixel *buffer, PanelType panel_type)
 : width(width), height(height), front_buffer(buffer), back_buffer(buffer + width * height)
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

    if (panel_type == PANEL_FM6126A) {
        FM6126A_setup();
    }
}

void Hub75::set_rgb(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    int offset = 0;
    if(y >= height / 2) {
        y -= height / 2;
        offset = (y * width + x) * 2;
        offset += 1;
    } else {
        offset = (y * width + x) * 2;
    }
    front_buffer[offset] = Pixel(r, g, b);
}

void Hub75::set_hsv(uint8_t x, uint8_t y, float h, float s, float v) {
    int offset = 0;
    if(y >= height / 2) {
        y -= height / 2;
        offset = (y * width + x) * 2;
        offset += 1;
    } else {
        offset = (y * width + x) * 2;
    }
    front_buffer[offset] = hsv_to_rgb(h, s, v);
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
    running = true;

    if(handler) {
        dma_channel = 0;

        if(pio_sm_is_claimed(pio, sm_data) || pio_sm_is_claimed(pio, sm_row)) {
            irq_set_enabled(pio_get_dreq(pio, sm_data, true), false);

            pio_sm_set_enabled(pio, sm_data, false);
            pio_sm_unclaim(pio, sm_data);
            pio_sm_set_enabled(pio, sm_row, false);
            pio_sm_unclaim(pio, sm_row);
            pio_clear_instruction_memory(pio);
        }

        pio_sm_claim(pio, sm_data);
        pio_sm_claim(pio, sm_row);

        data_prog_offs = pio_add_program(pio, &hub75_data_rgb888_program);
        row_prog_offs = pio_add_program(pio, &hub75_row_program);
        hub75_data_rgb888_program_init(pio, sm_data, data_prog_offs, DATA_BASE_PIN, pin_clk);
        hub75_row_program_init(pio, sm_row, row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, pin_stb);

        if(dma_channel_is_claimed(dma_channel)) {
            irq_set_enabled(DMA_IRQ_0, false);
            dma_channel_set_irq0_enabled(dma_channel, false);
            irq_set_enabled(pio_get_dreq(pio, sm_data, true), false);
            irq_remove_handler(DMA_IRQ_0, handler);
            dma_channel_wait_for_finish_blocking(dma_channel);
            dma_channel_unclaim(dma_channel);
        }

        dma_channel_claim(dma_channel);
        dma_channel_config config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
        channel_config_set_bswap(&config, false);
        channel_config_set_dreq(&config, pio_get_dreq(pio, sm_data, true));
        dma_channel_configure(dma_channel, &config, &pio->txf[sm_data], NULL, 0, false);
        dma_channel_set_irq0_enabled(dma_channel, true);
        irq_set_enabled(pio_get_dreq(pio, sm_data, true), true);
        irq_set_exclusive_handler(DMA_IRQ_0, handler);
        irq_set_enabled(DMA_IRQ_0, true);

        row = 0;
        bit = 0;

        dma_channel_set_trans_count(dma_channel, width * 4, false);
        dma_channel_set_read_addr(dma_channel, &back_buffer, true);
    } else {
        while (running) {
            display_update();
        }
    }
}

void Hub75::stop(irq_handler_t handler) {
    running = false;

    if(dma_channel_is_claimed(dma_channel)) {
        // stop and release the dma channel
        irq_set_enabled(DMA_IRQ_0, false);
        dma_channel_set_irq0_enabled(dma_channel, false);
        irq_set_enabled(pio_get_dreq(pio, sm_data, true), false);
        irq_remove_handler(DMA_IRQ_0, handler);

        dma_channel_wait_for_finish_blocking(dma_channel);
        dma_channel_unclaim(dma_channel);
    }

    hub75_wait_tx_stall(pio, sm_row);

    pio_sm_set_enabled(pio, sm_data, false);
    pio_sm_set_enabled(pio, sm_row, false);

    // release the pio and sm
    if(pio_sm_is_claimed(pio, sm_data)) pio_sm_unclaim(pio, sm_data);
    if(pio_sm_is_claimed(pio, sm_data)) pio_sm_unclaim(pio, sm_row);
    pio_clear_instruction_memory(pio);

    gpio_put(pin_oe, !oe_polarity);
    gpio_put(pin_stb, !stb_polarity);
}

Hub75::~Hub75() {
}

void Hub75::clear() {
    for(auto x = 0u; x < width; x++) {
        for(auto y = 0u; y < height; y++) {
            set_rgb(x, y, 0, 0, 0);
        }
    }
}

void Hub75::flip() {
    do_flip = true;
    while(do_flip) {};
}

void Hub75::display_update() {
    if (do_flip) {
        memcpy(back_buffer, front_buffer, width * height * sizeof(Pixel));
        do_flip = false;
    }

    for(auto bit = 1u; bit < 1 << 11; bit <<= 1) {
        for(auto y = 0u; y < height / 2; y++) {
            auto row_top = y * width;
            auto row_bottom = (y + height / 2) * width;
            for(auto x = 0u; x < width; x++) {
                Pixel pixel_top     = back_buffer[row_top + x];
                Pixel pixel_bottom  = back_buffer[row_bottom + x];

                gpio_put(pin_clk, !clk_polarity);

                gpio_put(pin_r0, (bool)(pixel_top.r & bit));
                gpio_put(pin_g0, (bool)(pixel_top.g & bit));
                gpio_put(pin_b0, (bool)(pixel_top.b & bit));

                gpio_put(pin_r1, (bool)(pixel_bottom.r & bit));
                gpio_put(pin_g1, (bool)(pixel_bottom.g & bit));
                gpio_put(pin_b1, (bool)(pixel_bottom.b & bit));

                gpio_put(pin_clk, clk_polarity);
            }

            gpio_put_masked(0b11111 << pin_row_a, y << pin_row_a);
            gpio_put(pin_stb, stb_polarity);
            gpio_put(pin_oe, oe_polarity);

            for(auto s = 0u; s < bit; ++s) {
                asm volatile("nop \nnop");
            }

            gpio_put(pin_stb, !stb_polarity);
            gpio_put(pin_oe, !oe_polarity);
        }
    }
}

void Hub75::dma_complete() {
    if (do_flip && bit == 0 && row == 0) {
        memcpy(back_buffer, front_buffer, width * height * sizeof(Pixel));
        do_flip = false;
    }

    if(dma_channel_get_irq0_status(dma_channel)) {
        dma_channel_acknowledge_irq0(dma_channel);

        // SM is finished when it stalls on empty TX FIFO (or, y'know, DMA callback)
        hub75_wait_tx_stall(pio, sm_data);

        // Check that previous OEn pulse is finished, else things WILL get out of sequence
        hub75_wait_tx_stall(pio, sm_row);

        // Latch row data, pulse output enable for new row.
        pio_sm_put_blocking(pio, sm_row, row | (3u * (1u << bit) << 5));

        row++;

        if(row == height / 2) {
            row = 0;
            bit++;
            if (bit == 12) {
                bit = 0;
            }
            hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
        }
    }

    dma_channel_set_trans_count(dma_channel, width * 4, false);
    dma_channel_set_read_addr(dma_channel, &back_buffer[row * width * 2], true);
}