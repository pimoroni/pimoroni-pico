#include "breakout_rgbmatrix5x5.hpp"

namespace pimoroni {

    bool BreakoutRGBMatrix5x5::init() {
        bool success = IS31FL3731::init();
        if(success) {
            enable({
                0b00000000, 0b10111111,
                0b00111110, 0b00111110,
                0b00111111, 0b10111110,
                0b00000111, 0b10000110,
                0b00110000, 0b00110000,
                0b00111111, 0b10111110,
                0b00111111, 0b10111110,
                0b01111111, 0b11111110,
                0b01111111, 0b00000000
            }, 0);
        }
        return success;
    }

    RGBLookup BreakoutRGBMatrix5x5::lookup_pixel(uint8_t index) {
        return lookup_table[index];
    }

    void BreakoutRGBMatrix5x5::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
        if (x == 1 || x == 3) {
            y = 4 - y;
        }
        uint8_t index = y + (x * 5);
        RGBLookup rgb = lookup_pixel(index);
        set(rgb.r, r);
        set(rgb.g, g);
        set(rgb.b, b);
    }

}
