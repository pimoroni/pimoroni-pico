#pragma once

#include "../../drivers/is31fl3731/is31fl3731.hpp"

namespace pimoroni {
    struct RGBLookup {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    class BreakoutRGBMatrix5x5 : public IS31FL3731 {
      public:
        void init();
        void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
      private:
        RGBLookup lookup_pixel(uint8_t index);

        // This wonderful lookup table maps the LEDs on RGB Matrix 5x5
        // from their 3x5x5 (remember, they're RGB) configuration to
        // their specific location in the 144 pixel buffer.
        const RGBLookup lookup_table[28] = {
            {118, 69, 85},
            {117, 68, 101},
            {116, 84, 100},
            {115, 83, 99},
            {114, 82, 98},
            {113, 81, 97},
            {112, 80, 96},
            {134, 21, 37},
            {133, 20, 36},
            {132, 19, 35},
            {131, 18, 34},
            {130, 17, 50},
            {129, 33, 49},
            {128, 32, 48},
            {127, 47, 63},
            {121, 41, 57},
            {122, 25, 58},
            {123, 26, 42},
            {124, 27, 43},
            {125, 28, 44},
            {126, 29, 45},
            {15, 95, 111},
            {8, 89, 105},
            {9, 90, 106},
            {10, 91, 107},
            {11, 92, 108},
            {12, 76, 109},
            {13, 77, 93},
        };
    };
}
