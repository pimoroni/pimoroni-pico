#pragma once
#include "pico/stdlib.h"

#include "ws2812.hpp"
#include "servo.hpp"
#include "servo_cluster.hpp"

namespace servo2040 {
  const uint SERVO_1 = 0;
  const uint SERVO_2 = 1;
  const uint SERVO_3 = 2;
  const uint SERVO_4 = 3;
  const uint SERVO_5 = 4;
  const uint SERVO_6 = 5;
  const uint SERVO_7 = 6;
  const uint SERVO_8 = 7;
  const uint SERVO_9 = 8;
  const uint SERVO_10 = 9;
  const uint SERVO_11 = 10;
  const uint SERVO_12 = 11;
  const uint SERVO_13 = 12;
  const uint SERVO_14 = 13;
  const uint SERVO_15 = 14;
  const uint SERVO_16 = 15;
  const uint SERVO_17 = 16;
  const uint SERVO_18 = 17;

  const uint LED_DATA = 18;

  const uint ADC_ADDR_0 = 22;
  const uint ADC_ADDR_1 = 24;
  const uint ADC_ADDR_2 = 25;

  const uint USER_SW = 23;

  const uint SENSE = 29; // The pin used for the board's sensing features

  constexpr float ADC_GAIN = 69;
  constexpr float ADC_OFFSET = 0.0145f;
  constexpr float SHUNT_RESISTOR = 0.003f;
}