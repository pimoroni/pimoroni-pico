#pragma once
#include "pico/stdlib.h"

#include "motor2.hpp"

namespace motor {
  namespace pico_motor_shim {
    const uint BUTTON_A = 2;

    const uint MOTOR_1P = 6;
    const uint MOTOR_1N = 7;
    const uint MOTOR_2P = 27;
    const uint MOTOR_2N = 26;

    const pin_pair MOTOR_1(MOTOR_1P, MOTOR_1N);
    const pin_pair MOTOR_2(MOTOR_2P, MOTOR_2N);
    const uint NUM_MOTORS = 2;
  }
}