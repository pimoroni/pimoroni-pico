#pragma once

#include "drivers/motor/motor.hpp"

namespace pimoroni {

  class PicoExplorer {
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 240;
    static const uint A = 12;
    static const uint B = 13;
    static const uint X = 14;
    static const uint Y = 15;

    static const uint8_t ADC0 = 0;
    static const uint8_t ADC1 = 1;
    static const uint8_t ADC2 = 2;

    static const uint ADC0_PIN = 26;
    static const uint ADC1_PIN = 27;
    static const uint ADC2_PIN = 28;

    static constexpr pin_pair MOTOR1_PINS{9, 8};
    static constexpr pin_pair MOTOR2_PINS{11, 10};

    static const uint8_t MOTOR1 = 0;
    static const uint8_t MOTOR2 = 1;

    static const uint8_t FORWARD = 0;
    static const uint8_t REVERSE = 1;
    static const uint8_t STOP    = 2;

    static const uint GP0 = 0;
    static const uint GP1 = 1;
    static const uint GP2 = 2;
    static const uint GP3 = 3;
    static const uint GP4 = 4;
    static const uint GP5 = 5;
    static const uint GP6 = 6;
    static const uint GP7 = 7;
  };

}