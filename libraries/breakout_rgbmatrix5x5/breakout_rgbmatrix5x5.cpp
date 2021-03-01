#include "breakout_rgbmatrix5x5.hpp"

namespace pimoroni {

  // This wonderful lookup table maps the LEDs on RGB Matrix 5x5
  // from their 3x5x5 (remember, they're RGB) configuration to
  // their specific location in the 144 pixel buffer.
  const RGBLookup BreakoutRGBMatrix5x5::LOOKUP_TABLE[] = {
    {118, 69, 85},
    {117, 68, 101},
    {116, 84, 100},
    {115, 83, 99},
    {114, 82, 98},

    {132, 19, 35},
    {133, 20, 36},
    {134, 21, 37},
    {112, 80, 96},
    {113, 81, 97},      

    {131, 18, 34},
    {130, 17, 50},
    {129, 33, 49},
    {128, 32, 48},
    {127, 47, 63},

    {125, 28, 44},
    {124, 27, 43},
    {123, 26, 42},
    {122, 25, 58},
    {121, 41, 57},

    {126, 29, 45},
    {15, 95, 111},
    {8, 89, 105},
    {9, 90, 106},
    {10, 91, 107},
};

  bool BreakoutRGBMatrix5x5::init() {
    matrix.init();
    matrix.enable({
      0b00000000, 0b10111111,
      0b00111110, 0b00111110,
      0b00111111, 0b10111110,
      0b00000111, 0b10000110,
      0b00110000, 0b00110000,
      0b00111111, 0b10111110,
      0b00111111, 0b10111110,
      0b01111111, 0b11111110,
      0b01111111, 0b00000000
    }, 0);

    return true;
  }

  i2c_inst_t* BreakoutRGBMatrix5x5::get_i2c() const {
    return matrix.get_i2c();
  }

  int BreakoutRGBMatrix5x5::get_sda() const {
    return matrix.get_sda();
  }

  int BreakoutRGBMatrix5x5::get_scl() const {
    return matrix.get_scl();
  }

  void BreakoutRGBMatrix5x5::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t index = x + (y * WIDTH);
    RGBLookup rgb = lookup_pixel(index);
    matrix.set(rgb.r, r);
    matrix.set(rgb.g, g);
    matrix.set(rgb.b, b);
  }

  void BreakoutRGBMatrix5x5::update() {
    matrix.update();
  }

  void BreakoutRGBMatrix5x5::clear() {
    matrix.clear();
  }

  RGBLookup BreakoutRGBMatrix5x5::lookup_pixel(uint8_t index) {
    return LOOKUP_TABLE[index];
  }
}