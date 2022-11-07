#pragma once

#include "drivers/st7567/st7567.hpp"
#include "drivers/button/button.hpp"
#include "drivers/rgbled/rgbled.hpp"

namespace pimoroni {

  namespace GfxPack{
    static const SPIPins gfx_pack_pins = {PIMORONI_SPI_DEFAULT_INSTANCE, 17, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, 9};
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
