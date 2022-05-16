#pragma once
#include "pico/stdlib.h"

#include "motor.hpp"

namespace motor {
  namespace pico_motor_shim {
    const uint BUTTON_A = 2;

    const uint MOTOR_1_P = 6;
    const uint MOTOR_1_N = 7;
    const uint MOTOR_2_P = 27;
    const uint MOTOR_2_N = 26;

    const pin_pair MOTOR_1(MOTOR_1_P, MOTOR_1_N);
    const pin_pair MOTOR_2(MOTOR_2_P, MOTOR_2_N);
    const uint NUM_MOTORS = 2;
  }
}