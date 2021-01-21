#pragma once

#include <string>

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class AS7262 {
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = 0x49;
    int8_t sda       = 4;
    int8_t scl       = 5;
    int8_t interrupt = 22;

  public:
    struct reading {
        float red;
        float orange;
        float yellow;
        float green;
        float blue;
        float violet;
    };

    enum reg {
        DEVICE      = 0x00,
        HW_VERSION  = 0x01,
        FW_VERSION  = 0x02, // + 0x03
        CONTROL     = 0x04,
        INT_T       = 0x05,
        TEMP        = 0x06,
        LED_CONTROL = 0x07,
        V_HIGH      = 0x08, // Violet
        V_LOW       = 0x09,
        B_HIGH      = 0x0A, // Blue
        B_LOW       = 0x0B,
        G_HIGH      = 0x0C, // Green
        G_LOW       = 0x0D,
        Y_HIGH      = 0x0E, // Yellow
        Y_LOW       = 0x0F,
        O_HIGH      = 0x10, // Orange
        O_LOW       = 0x11,
        R_HIGH      = 0x12, // Red
        R_LOW       = 0x13,
        V_CAL_F     = 0x14, // -> 0x17 Float (Violet)
        B_CAL_F     = 0x18, // -> 0x1B Float (Blue)
        G_CAL_F     = 0x1C, // -> 0x1F Float (Green)
        Y_CAL_F     = 0x20, // -> 0x23 Float (Yellow)
        O_CAL_F     = 0x24, // -> 0x27 Float (Orange)
        R_CAL_F     = 0x28, // -> 0x27 Float (Red)
    };

    enum class gain : uint8_t {
        X1   = 0b00,
        X3_7 = 0b01,
        X16  = 0b10,
        X64  = 0b11
    };

    enum class illumination_current : uint8_t {
        ma12  = 0b00,
        ma25  = 0b01,
        ma50  = 0b10,
        ma100 = 0b11
    };

    enum class indicator_current : uint8_t {
        ma1 = 0b00,
        ma2 = 0b01,
        ma4 = 0b10,
        ma8 = 0b11,
    };

    enum class measurement_mode : uint8_t {
        cont_ygnv   = 0b00, // yellow, green, blue, violet - continuous
        cont_royg   = 0b01, // red, orange, yellow, green - continuous
        cont_roygbr = 0b10, // red, orange, yellow, green, violet - continuous
        oneshot     = 0b11  // everything - one-shot
    };

    AS7262() {}

    AS7262(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}

    void init();
    void reset();

    uint8_t device_type();
    uint8_t hardware_version();
    std::string firmware_version();
    reading read();
    uint8_t temperature();

    void set_gain(gain gain);
    void set_measurement_mode(measurement_mode mode);
    void set_indicator_current(indicator_current current);
    void set_illumination_current(illumination_current current);
    void set_leds(bool illumination, bool indicator);

    // Virtual i2c transfers, routed through read/write/status regs
    uint8_t i2c_reg_read_uint8(uint8_t reg);
    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    uint16_t i2c_reg_read_uint16(uint8_t reg);
    float i2c_reg_read_float(uint8_t reg);

  private:
    bool data_ready();
    uint8_t _i2c_status(); 
    int _i2c_read(uint8_t reg, uint8_t *values, int len);
    int _i2c_write(uint8_t reg, uint8_t *values, int len);

    // *Real* single-byte i2c transfers
    uint8_t _i2c_reg_read_uint8(uint8_t reg);
    void _i2c_reg_write_uint8(uint8_t reg, uint8_t value);
  };

}
