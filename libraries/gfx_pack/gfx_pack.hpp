#pragma once

#include "pico/stdlib.h"
#include "common/pimoroni_bus.hpp"

namespace pimoroni {

  SPIPins gfx_pack_pins= {PIMORONI_SPI_DEFAULT_INSTANCE, 17, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, 9};

  class GfxPack{

  public:
    static const int WIDTH = 128;
    static const int HEIGHT = 64;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t C = 14;
    static const uint8_t D = 15;
    static const uint8_t E = 22;
    static const uint8_t BL_R = 6;
    static const uint8_t BL_G = 7;
    static const uint8_t BL_B = 8;
    static const uint8_t BL_W = 9;
  };
}
