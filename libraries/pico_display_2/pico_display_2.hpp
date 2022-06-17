#pragma once

#include "pico/stdlib.h"

namespace pimoroni {
  class PicoDisplay2 {
  public:
    static const int WIDTH = 320;
    static const int HEIGHT = 240;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;
    static const uint8_t LED_R = 6;
    static const uint8_t LED_G = 7;
    static const uint8_t LED_B = 8;
  };
}
