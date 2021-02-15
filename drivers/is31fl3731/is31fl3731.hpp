#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <initializer_list>

namespace pimoroni {

  class IS31FL3731 {
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = 0x74;
    int8_t sda       = 4;
    int8_t scl       = 5;

    uint8_t buf[145];

  public:
    IS31FL3731() {}

    IS31FL3731(i2c_inst_t *i2c, uint8_t sda, uint8_t scl) :
      i2c(i2c), sda(sda), scl(scl) {}

    void init();
    void enable(std::initializer_list<uint8_t> pattern, uint8_t frame = 0);
    void set(uint8_t index, uint8_t brightness);
    void update(uint8_t frame = 0);
    void clear();

    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    int16_t i2c_reg_read_int16(uint8_t reg);
  };

}
