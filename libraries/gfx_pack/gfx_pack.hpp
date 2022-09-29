#pragma once

#include "pico/stdlib.h"

namespace pimoroni {
  class gfx_pack {
  public:
    static const int WIDTH = 128;
    static const int HEIGHT = 64;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;
    static const uint8_t BL_R = 6;
    static const uint8_t BL_G = 7;
    static const uint8_t BL_B = 8;
    static const uint8_t BL_W = 9;
  };
}
