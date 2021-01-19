#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  class PicoUnicorn {
  public:
    static const int WIDTH = 16;
    static const int HEIGHT = 7;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;
  private:
    uint32_t __fb[WIDTH * HEIGHT];
    PIO bitstream_pio = pio0;
    uint bitstream_sm = 0;

  public:
    void init();

    void clear();
    void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(uint8_t x, uint8_t y, uint8_t v);

    bool is_pressed(uint8_t button);
  };

}