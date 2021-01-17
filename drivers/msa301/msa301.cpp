#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "msa301.hpp"

namespace pimoroni {

  enum reg {
    RESET     = 0x00,
    X_AXIS    = 0x02,
    Y_AXIS    = 0x04,
    Z_AXIS    = 0x06
    // todo: lots of other features
  };

  void MSA301::init() {
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    reset();

    i2c_reg_write_uint8(0x11, 0x00); // set power mode
    i2c_reg_write_uint8(0x0f, 0x00); // set range & resolution
  }

  void MSA301::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
  }

  int16_t MSA301::i2c_reg_read_int16(uint8_t reg) {
    int16_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 2, false);
    return value;
  }

  void MSA301::reset() {
    i2c_reg_write_uint8(reg::RESET, 0b00100100);
    sleep_ms(1);
  }

  float MSA301::get_axis(uint8_t axis, uint8_t sample_count) {
    if(sample_count > 1) {
      int32_t total = 0;
      for(uint8_t i = 0; i < sample_count; i++) {
        total += i2c_reg_read_int16(axis);
      }
      total /= sample_count;
      return total / 16384.0f;
    }

    return i2c_reg_read_int16(axis) / 16384.0f;
  }

}