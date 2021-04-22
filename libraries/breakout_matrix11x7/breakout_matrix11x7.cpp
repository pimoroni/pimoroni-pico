#include "breakout_matrix11x7.hpp"

namespace pimoroni {
    void BreakoutMatrix11x7::init() {
        IS31FL3731::init();
        enable({
            0b01111111, 0b01111111,
            0b01111111, 0b01111111,
            0b01111111, 0b01111111,
            0b01111111, 0b01111111,
            0b01111111, 0b01111111,
            0b01111111, 0b00000000,
            0b00000000, 0b00000000,
            0b00000000, 0b00000000,
            0b00000000, 0b00000000,
        }, 0);
    }

    uint8_t BreakoutMatrix11x7::lookup_pixel(uint8_t index) {
        return lookup_table[index];
    }

    void BreakoutMatrix11x7::set_pixel(uint8_t x, uint8_t y, uint8_t c) {
        uint8_t i = lookup_pixel(y * WIDTH + x);
        set(i, c);
    }

}
