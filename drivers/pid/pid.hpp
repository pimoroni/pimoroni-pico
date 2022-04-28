#pragma once

//#include <stdint.h>
#include "pico/stdlib.h"
//include "common/pimoroni_common.hpp"

namespace pimoroni {

  class PID {
  public:
    PID();
    PID(float kp, float ki, float kd, float sample_rate);

    float calculate(float value);
    float calculate(float value, float value_change);

  public:
    float kp;
    float ki;
    float kd;
    float setpoint;
  private:
    float error_sum;
    float last_value;
    float sample_rate;
  };

}