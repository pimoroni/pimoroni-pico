#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define REG_SYSTEM_CONTROL 0x40
#define REG_MODE_CONTROL1 0x41
#define REG_MODE_CONTROL2 0x42
#define REG_MODE_CONTROL3 0x44
#define REG_COLOUR_DATA 0x50
#define REG_DINT_DATA 0x58
#define REG_INTERRUPT 0x60
#define REG_PERSISTENCE 0x61
#define REG_THRESHOLD_LOW 0x64
#define REG_THRESHOLD_HIGH 0x62
#define REG_MANUFACTURER 0x92

#define CHIP_ID 0b001011
#define MANUFACTURER 0xe0

#define I2C_ADDR 0x38
#define I2C_ADDR_ALT 0x39

namespace pimoroni {
    typedef struct  {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t c;
    } rgbc_t;

    class BH1745 {
        public:
            BH1745() {};

            BH1745(uint8_t addr) : address(addr) {};

            BH1745(i2c_inst_t *i2c, uint8_t addr, uint8_t sda, uint8_t scl, uint8_t interrupt) :
                i2c(i2c), address(addr), sda(sda), scl(scl), interrupt(interrupt) {};

            int init();
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
            i2c_inst_t *i2c = i2c0;

            // interface pins with our standard defaults where appropriate
            int8_t address   = 0x38;
            int8_t sda       = 4;
            int8_t scl       = 5;
            int8_t interrupt = 22;

            float channel_compensation[4] = {2.2f, 1.0f, 1.8f, 10.0f};

            // From i2cdevice
            int write_bytes(uint8_t reg, uint8_t *buf, int len);
            int read_bytes(uint8_t reg, uint8_t *buf, int len);
            uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
            void set_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
            void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    };
}