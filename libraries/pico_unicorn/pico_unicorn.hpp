#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  class PicoUnicorn {
    uint32_t __fb[16 * 7];
    PIO bitstream_pio = pio0;
    uint sm = 0;

  public:
    void init();
    void update();
    void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
  };

}