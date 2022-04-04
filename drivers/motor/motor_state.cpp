#include "motor_state.hpp"
#include "float.h"

namespace motor {
  MotorState::MotorState() 
    : motor_speed(0.0f), motor_scale(DEFAULT_SPEED_SCALE), inverted(false), last_enabled_duty(0.0f), enabled(false) {
  }

  MotorState::MotorState(float speed_scale, bool inverted)
    : motor_speed(0.0f), motor_scale(MAX(speed_scale, FLT_EPSILON)), inverted(inverted), last_enabled_duty(0.0f), enabled(false) {
  }

  float MotorState::enable_with_return() {
    enabled = true;

    float duty = 0.0f;
    if((duty <= 0.0f - deadzone_percent) || (duty >= deadzone_percent)) {
      if(inverted)
        duty = 0.0f - last_enabled_duty;
      else
        duty = last_enabled_duty;
    }
    return duty;
  }

  float MotorState::disable_with_return() {
    enabled = false;
    return 0.0f; // A zero duty
  }

  bool MotorState::is_enabled() const {
    return enabled;
  }

  float MotorState::get_duty() const {
    return last_enabled_duty;
  }

  float MotorState::set_duty_with_return(float duty) {
    // Clamp the duty between the hard limits
    last_enabled_duty = MIN(MAX(duty, -1.0f), 1.0f);
    motor_speed = last_enabled_duty * motor_scale;

    return enable_with_return();
  }

  float MotorState::get_speed() const {
    return motor_speed;
  }

  float MotorState::set_speed_with_return(float speed) {
    // Clamp the speed between the hard limits
    motor_speed = MIN(MAX(speed, -motor_scale), motor_scale);
    last_enabled_duty = motor_speed / motor_scale;

    return enable_with_return();
  }

  float MotorState::get_speed_scale() const {
    return motor_scale;
  }

  void MotorState::set_speed_scale(float speed_scale) {
    motor_scale = MAX(speed_scale, FLT_EPSILON);
    motor_speed = last_enabled_duty * motor_scale;
  }

  void MotorState::invert_direction(bool invert) {
    if(invert != inverted) {
      inverted = invert;
      motor_speed = 0.0f - motor_speed;
      last_enabled_duty = 0.0f - last_enabled_duty;
    }
  }

  bool MotorState::is_inverted() const {
    return inverted;
  }

  float MotorState::stop_with_return() {
    return set_duty_with_return(0.0f);
  }

  float MotorState::to_full_negative_with_return() {
    return set_duty_with_return(-1.0f);
  }

  float MotorState::to_full_positive_with_return() {
    return set_duty_with_return(1.0f);
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max) {
    float speed = MotorState::map_float(in, in_min, in_max, 0.0f - get_speed_scale(), get_speed_scale());
    return set_speed_with_return(speed);
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max) {
    float speed = MotorState::map_float(in, in_min, in_max, speed_min, speed_max);
    return set_speed_with_return(speed);
  }

  int32_t MotorState::duty_to_level(float duty, uint32_t resolution) {
    return (int32_t)(duty * (float)resolution);
  }

  float MotorState::map_float(float in, float in_min, float in_max, float out_min, float out_max) {
    return (((in - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
  }

};