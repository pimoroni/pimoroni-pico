#pragma once

#include "drivers/is31fl3731/is31fl3731.hpp"

namespace pimoroni {
    class BreakoutMatrix11x7 : public IS31FL3731 {
      public:
        static constexpr uint8_t WIDTH = 11;
        static constexpr uint8_t HEIGHT = 7;
        static constexpr int8_t DEFAULT_I2C_ADDRESS    = 0x75;
        static constexpr int8_t ALTERNATE_I2C_ADDRESS  = 0x77;

        bool init();

        BreakoutMatrix11x7() : IS31FL3731(new I2C(I2C_BG_SDA, I2C_BG_SCL), DEFAULT_I2C_ADDRESS) {}

        BreakoutMatrix11x7(uint8_t address) : IS31FL3731(new I2C(I2C_DEFAULT_SDA, I2C_DEFAULT_SCL), address) {}
      
        BreakoutMatrix11x7(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS) : IS31FL3731(i2c, address) {}

        BreakoutMatrix11x7(i2c_inst_t *i2c_inst, uint8_t address, uint8_t sda, uint8_t scl) : IS31FL3731(i2c_inst, address, sda, scl) {}

        void set_pixel(uint8_t x, uint8_t y, uint8_t c);

      private:
        uint8_t lookup_pixel(uint8_t index);

        // This wonderful lookup table maps the LEDs on Matrix 11x7
        const uint8_t lookup_table[WIDTH * HEIGHT] = {
            6, 22, 38, 54, 70, 86, 14, 30, 46, 62, 78,
            5, 21, 37, 53, 69, 85, 13, 29, 45, 61, 77,
            4, 20, 36, 52, 68, 84, 12, 28, 44, 60, 76,
            3, 19, 35, 51, 67, 83, 11, 27, 43, 59, 75,
            2, 18, 34, 50, 66, 82, 10, 26, 42, 58, 74,
            1, 17, 33, 49, 65, 81, 9, 25, 41, 57, 73,
            0, 16, 32, 48, 64, 80, 8, 24, 40, 56, 72
        };
    };
}
