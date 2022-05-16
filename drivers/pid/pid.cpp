#include "pid.hpp"

namespace pimoroni {
  PID::PID()
  : kp(0.0f), ki(0.0f), kd(0.0f), setpoint(0.0f)
  , error_sum(0.0f), last_value(0.0f), sample_rate(1.0f) {
  }

  PID::PID(float kp, float ki, float kd, float sample_rate)
  : kp(kp), ki(ki), kd(kd), setpoint(0.0f)
  , error_sum(0.0f), last_value(0.0f), sample_rate(sample_rate) {
  }

  float PID::calculate(float value) {
    float error = setpoint - value;
    error_sum += error * sample_rate;
    float rate_error = (value - last_value) / sample_rate;
    last_value = value;

    return (error * kp) + (error_sum * ki) - (rate_error * kd);
  }

  float PID::calculate(float value, float value_change) {
    float error = setpoint - value;
    error_sum += error * sample_rate;
    last_value = value;

    return (error * kp) + (error_sum * ki) - (value_change * kd);
  }
}