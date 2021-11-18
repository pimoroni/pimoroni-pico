#include <cstring>
#include <algorithm>

#include "hub75.hpp"
#include "pico/stdlib.h"
#include "pico/multicore.h"


Hub75::Hub75(uint8_t width, uint8_t height, Pixel *buffer)
 : width(width), height(height), front_buffer(buffer), back_buffer(buffer + width * height)
 {
    // 1.3v allows overclock to ~280000-300000 but YMMV. Faster clock = faster screen update rate!
    // vreg_set_voltage(VREG_VOLTAGE_1_30);
    // sleep_ms(100);

    // 200MHz is roughly about the lower limit for driving a 64x64 display smoothly.
    // Just don't look at it out of the corner of your eye.
    //set_sys_clock_khz(200000, true);

    // Set up allllll the GPIO
    gpio_init(pin_r0); gpio_set_function(pin_r0, GPIO_FUNC_SIO); gpio_set_dir(pin_r0, true);
    gpio_init(pin_g0); gpio_set_function(pin_g0, GPIO_FUNC_SIO); gpio_set_dir(pin_g0, true);
    gpio_init(pin_b0); gpio_set_function(pin_b0, GPIO_FUNC_SIO); gpio_set_dir(pin_b0, true);

    gpio_init(pin_r1); gpio_set_function(pin_r1, GPIO_FUNC_SIO); gpio_set_dir(pin_r1, true);
    gpio_init(pin_g1); gpio_set_function(pin_g1, GPIO_FUNC_SIO); gpio_set_dir(pin_g1, true);
    gpio_init(pin_b1); gpio_set_function(pin_b1, GPIO_FUNC_SIO); gpio_set_dir(pin_b1, true);

    gpio_init(pin_row_a); gpio_set_function(pin_row_a, GPIO_FUNC_SIO); gpio_set_dir(pin_row_a, true);
    gpio_init(pin_row_b); gpio_set_function(pin_row_b, GPIO_FUNC_SIO); gpio_set_dir(pin_row_b, true);
    gpio_init(pin_row_c); gpio_set_function(pin_row_c, GPIO_FUNC_SIO); gpio_set_dir(pin_row_c, true);
    gpio_init(pin_row_d); gpio_set_function(pin_row_d, GPIO_FUNC_SIO); gpio_set_dir(pin_row_d, true);
    gpio_init(pin_row_e); gpio_set_function(pin_row_e, GPIO_FUNC_SIO); gpio_set_dir(pin_row_e, true);

    gpio_init(pin_clk); gpio_set_function(pin_clk, GPIO_FUNC_SIO); gpio_set_dir(pin_clk, true);
    gpio_init(pin_stb); gpio_set_function(pin_stb, GPIO_FUNC_SIO); gpio_set_dir(pin_stb, true);
    gpio_init(pin_oe); gpio_set_function(pin_oe, GPIO_FUNC_SIO); gpio_set_dir(pin_oe, true);
}

void Hub75::set_rgb(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    front_buffer[y * width + x] = Pixel(r, g, b);
}

void Hub75::FM6126A_write_register(uint16_t value, uint8_t position) {
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

void Hub75::start() {
    running = true;

    // Ridiculous register write nonsense for the FM6126A-based 64x64 matrix
    FM6126A_write_register(0b1111111111111110, 12);
    FM6126A_write_register(0b0000001000000000, 13);

    while (running) {
        display_update();
    }
}

void Hub75::stop() {
    running = false;
}

void Hub75::clear() {

}

void Hub75::flip() {
    do_flip = true;
}

void Hub75::display_update() {
    if (do_flip) {
        //std::swap(front_buffer, back_buffer);
        memcpy((uint8_t *)back_buffer, (uint8_t *)front_buffer, width * height * sizeof(Pixel));
        do_flip = false;
    }

    for(auto bit = 1u; bit < 1 << 11; bit <<= 1) {
        for(auto y = 0u; y < height / 2; y++) {
            for(auto x = 0u; x < width; x++) {
                Pixel pixel_top     = back_buffer[y * width + x];
                Pixel pixel_bottom  = back_buffer[(y + height / 2) * width + x];

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
        sleep_us(1);
    }
}