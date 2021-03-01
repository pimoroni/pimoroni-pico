#include "breakout_matrix11x7.hpp"

namespace pimoroni {

  // This wonderful lookup table maps the LEDs on Matrix 11x7
  // from their configuration to their specific location
  // in the 144 pixel buffer.
  const uint8_t BreakoutMatrix11x7::LOOKUP_TABLE[] = {
    6, 22, 38, 54, 70, 86, 14, 30, 46, 62, 78,
    5, 21, 37, 53, 69, 85, 13, 29, 45, 61, 77,
    4, 20, 36, 52, 68, 84, 12, 28, 44, 60, 76,
    3, 19, 35, 51, 67, 83, 11, 27, 43, 59, 75,
    2, 18, 34, 50, 66, 82, 10, 26, 42, 58, 74,
    1, 17, 33, 49, 65, 81, 9, 25, 41, 57, 73,
    0, 16, 32, 48, 64, 80, 8, 24, 40, 56, 72
   };

  bool BreakoutMatrix11x7::init() {
    matrix.init();
    matrix.enable({
      0b01111111, 0b01111111,
      0b01111111, 0b01111111,
      0b01111111, 0b01111111,
      0b01111111, 0b01111111,
      0b01111111, 0b01111111,
      0b01111111, 0b00000000,
      0b00000000, 0b00000000,
      0b00000000, 0b00000000,
      0b00000000, 0b00000000
    }, 0);

    return true;
  }

  i2c_inst_t* BreakoutMatrix11x7::get_i2c() const {
    return matrix.get_i2c();
  }

  int BreakoutMatrix11x7::get_sda() const {
    return matrix.get_sda();
  }

  int BreakoutMatrix11x7::get_scl() const {
    return matrix.get_scl();
  }

  void BreakoutMatrix11x7::set_pixel(uint8_t x, uint8_t y, uint8_t val) {
    uint8_t index = x + (y * WIDTH);
    uint8_t pixel = lookup_pixel(index);
    matrix.set(pixel, val);
  }

  void BreakoutMatrix11x7::update() {
    matrix.update();
  }

  void BreakoutMatrix11x7::clear() {
    matrix.clear();
  }

  uint8_t BreakoutMatrix11x7::lookup_pixel(uint8_t index) {
    return LOOKUP_TABLE[index];
  }
}