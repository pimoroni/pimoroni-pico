#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_i2c.hpp"

#define BH1745_REG_SYSTEM_CONTROL 0x40
#define BH1745_REG_MODE_CONTROL1 0x41
#define BH1745_REG_MODE_CONTROL2 0x42
#define BH1745_REG_MODE_CONTROL3 0x44
#define BH1745_REG_COLOUR_DATA 0x50
#define BH1745_REG_DINT_DATA 0x58
#define BH1745_REG_INTERRUPT 0x60
#define BH1745_REG_PERSISTENCE 0x61
#define BH1745_REG_THRESHOLD_LOW 0x64
#define BH1745_REG_THRESHOLD_HIGH 0x62
#define BH1745_REG_MANUFACTURER 0x92

#define BH1745_CHIP_ID 0b001011
#define BH1745_MANUFACTURER 0xe0

namespace pimoroni {
    typedef struct  {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t c;
    } rgbc_t;

    class BH1745 {
        public:
            static const uint8_t DEFAULT_I2C_ADDRESS      = 0x38;
            static const uint8_t I2C_ADDRESS_ALTERNATE    = 0x39;

            BH1745(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS) :
                i2c(i2c), address(address) {}

            BH1745(i2c_inst_t *i2c, uint8_t addr, uint8_t sda, uint8_t scl, uint interrupt) :
                BH1745(new I2C(sda, scl), address) {};

            bool init();

            I2C* get_i2c() const;
            int get_address() const;

            uint8_t get_chip_id();
            uint8_t get_manufacturer();
            void set_threshold_high(uint16_t value);
            void set_threshold_low(uint16_t value);
            void set_measurement_time_ms(uint16_t value);
            rgbc_t get_rgbc_raw();
            rgbc_t get_rgb_clamped();
            rgbc_t get_rgb_scaled();
            void reset();
            void set_leds(bool state=true);

        private:
            I2C *i2c;

            // interface pins with our standard defaults where appropriate
            int8_t address = DEFAULT_I2C_ADDRESS;
            uint interrupt = 22;

            float channel_compensation[4] = {2.2f, 1.0f, 1.8f, 10.0f};
    };
}