#pragma once
#include "pico/stdlib.h"

#include "apa102.hpp"
#include "ws2812.hpp"

namespace plasma {
  namespace plasma2040 {
    static const uint LED_R = 16;
    static const uint LED_G = 17;
    static const uint LED_B = 18;

    static const uint BUTTON_A = 12;
    static const uint BUTTON_B = 13;

    static const uint USER_SW = 23;

    static const uint CLK = 14; // Used only for APA102
    static const uint DAT = 15; // Used for both APA102 and WS2812

    static const uint SENSE = 29; // The pin used for current sensing

    static constexpr float ADC_GAIN = 50;
    static constexpr float SHUNT_RESISTOR = 0.015f;
  }
}