#pragma once

#include "drivers/hub75/hub75.hpp"
#include "drivers/button/button.hpp"
#include "drivers/rgbled/rgbled.hpp"

namespace pimoroni {

  namespace Interstate75{
    static const uint8_t A = 14;
    static const uint8_t B = 15;
    static const uint8_t BOOT = 23;
    static const uint8_t LED_R = 16;
    static const uint8_t LED_G = 17;
    static const uint8_t LED_B = 18;
  };
}
