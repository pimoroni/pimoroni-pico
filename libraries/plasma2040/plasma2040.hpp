#pragma once
#include "pico/stdlib.h"

#include "apa102.hpp"
#include "ws2812.hpp"

namespace plasma {
  namespace plasma2040 {
    const uint LED_R = 16;
    const uint LED_G = 17;
    const uint LED_B = 18;

    const uint BUTTON_A = 12;
    const uint BUTTON_B = 13;

    const uint USER_SW = 23;

    const uint CLK = 14; // Used only for APA102
    const uint DAT = 15; // Used for both APA102 and WS2812

    const uint CURRENT_SENSE = 29; // The pin used for current sensing

    constexpr float ADC_GAIN = 50;
    constexpr float SHUNT_RESISTOR = 0.015f;
  }
}