#include "bh1745.hpp"
#include <algorithm>

namespace pimoroni {
    bool BH1745::init() {
        reset();

        if (this->get_chip_id() != BH1745_CHIP_ID || this->get_manufacturer() != BH1745_MANUFACTURER) {
            return false;
        }

        reset();
        i2c->clear_bits(address, BH1745_REG_SYSTEM_CONTROL, 6);    // Clear INT reset bit
        set_measurement_time_ms(640);
        i2c->set_bits(address, BH1745_REG_MODE_CONTROL2, 4);       // Enable RGBC
        i2c->set_bits(address, BH1745_REG_MODE_CONTROL3, 0, 0xff); // Turn on sensor
        set_threshold_high(0x0000);                                // Set threshold so int will always fire
        set_threshold_low(0xFFFF);                                 // this lets us turn on the LEDs with the int pin
        i2c->clear_bits(address, BH1745_REG_INTERRUPT, 4);         // Enable interrupt latch

        sleep_ms(320);

        return true;
    }

    I2C* BH1745::get_i2c() const {
        return i2c;
    }

    int BH1745::get_address() const {
        return address;
    }

    uint8_t BH1745::get_chip_id() {
        uint8_t chip_id = i2c->reg_read_uint8(address, BH1745_REG_SYSTEM_CONTROL);
        return chip_id & 0b00111111;
    }

    uint8_t BH1745::get_manufacturer() {
        uint8_t manufacturer = i2c->reg_read_uint8(address, BH1745_REG_MANUFACTURER);
        return manufacturer;
    }

    void BH1745::reset() {
        i2c->set_bits(address, BH1745_REG_SYSTEM_CONTROL, 7);

        while (i2c->get_bits(address, BH1745_REG_SYSTEM_CONTROL, 7)) {
            sleep_ms(100);
        }
    }

    void BH1745::set_measurement_time_ms(uint16_t value) {
        uint8_t reg = 0;
        switch(value) {
            case 160:
                reg = 0b000;
                break;
            case 320:
                reg = 0b001;
                break;
            case 640:
                reg = 0b010;
                break;
            case 1280:
                reg = 0b011;
                break;
            case 2560:
                reg = 0b100;
                break;
            case 5120:
                reg = 0b101;
                break;
        }
        i2c->write_bytes(address, BH1745_REG_MODE_CONTROL1, &reg, 1);
    }

    void BH1745::set_threshold_high(uint16_t value) {
        i2c->write_bytes(address, BH1745_REG_THRESHOLD_HIGH, (uint8_t *)&value, 2);
    }

    void BH1745::set_threshold_low(uint16_t value) {
        i2c->write_bytes(address, BH1745_REG_THRESHOLD_LOW, (uint8_t *)&value, 2);
    }

    void BH1745::set_leds(bool state) {
        if(state){
            i2c->set_bits(address, BH1745_REG_INTERRUPT, 0);
        }
        else
        {
            i2c->clear_bits(address, BH1745_REG_INTERRUPT, 0);
        }
        
    }

    rgbc_t BH1745::get_rgb_scaled() {
        rgbc_t rgbc = this->get_rgbc_raw();

        if(rgbc.c > 0) {
            rgbc.r = (uint16_t)((uint32_t)rgbc.r * 255 / rgbc.c) & 0xff;
            rgbc.g = (uint16_t)((uint32_t)rgbc.g * 255 / rgbc.c) & 0xff;
            rgbc.b = (uint16_t)((uint32_t)rgbc.b * 255 / rgbc.c) & 0xff;
        } else {
            rgbc.r = 0;
            rgbc.g = 0;
            rgbc.b = 0;
        }

        return rgbc;
    }

    rgbc_t BH1745::get_rgb_clamped() {
        rgbc_t rgbc = get_rgbc_raw();

        uint16_t vmax = std::max(rgbc.r, std::max(rgbc.g, rgbc.b));

        rgbc.r = (uint16_t)((uint32_t)rgbc.r * 255 / vmax);
        rgbc.g = (uint16_t)((uint32_t)rgbc.g * 255 / vmax);
        rgbc.b = (uint16_t)((uint32_t)rgbc.b * 255 / vmax);

        return rgbc;
    }

    rgbc_t BH1745::get_rgbc_raw() {
        while(i2c->get_bits(address, BH1745_REG_MODE_CONTROL2, 7) == 0) {
            sleep_ms(1);
        }
        rgbc_t colour_data;
        i2c->read_bytes(address, BH1745_REG_COLOUR_DATA, (uint8_t *)&colour_data, 8);
        colour_data.r *= channel_compensation[0];
        colour_data.g *= channel_compensation[1];
        colour_data.b *= channel_compensation[2];
        colour_data.c *= channel_compensation[3];
        return colour_data;
    }
}