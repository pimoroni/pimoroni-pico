#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class MSA301 {
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = 0x26;
    int8_t sda       = 20;
    int8_t scl       = 21;
    int8_t interrupt = 22;

  public:
    MSA301() {}

    MSA301(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}

    void init();
    void reset();

    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    int16_t i2c_reg_read_int16(uint8_t reg);

    float get_axis(uint8_t axis, uint8_t sample_count = 1);

    const uint8_t X = 0x02;
    const uint8_t Y = 0x04;
    const uint8_t Z = 0x06;
  };

}
