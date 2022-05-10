#include "motor_state.hpp"
#include "common/pimoroni_common.hpp"
#include "float.h"
#include "math.h"

namespace motor {
  MotorState::MotorState()
    : motor_speed(0.0f), last_enabled_duty(0.0f), enabled(false)
    , motor_direction(NORMAL_DIR), motor_scale(DEFAULT_SPEED_SCALE)
    , motor_zeropoint(DEFAULT_ZEROPOINT), motor_deadzone(DEFAULT_DEADZONE) {
  }

  MotorState::MotorState(Direction direction, float speed_scale, float zeropoint, float deadzone)
    : motor_speed(0.0f), last_enabled_duty(0.0f), enabled(false)
    , motor_direction(direction) , motor_scale(MAX(speed_scale, FLT_EPSILON))
    , motor_zeropoint(CLAMP(zeropoint, 0.0f, 1.0f - FLT_EPSILON)), motor_deadzone(CLAMP(deadzone, 0.0f, 1.0f)) {
  }

  float MotorState::enable_with_return() {
    enabled = true;
    return get_deadzoned_duty();
  }

  float MotorState::disable_with_return() {
    enabled = false;
    return NAN;
  }

  bool MotorState::is_enabled() const {
    return enabled;
  }

  float MotorState::get_duty() const {
    return (motor_direction == NORMAL_DIR) ? last_enabled_duty : 0.0f - last_enabled_duty;
  }

  float MotorState::get_deadzoned_duty() const {
    float duty = 0.0f;
    if((last_enabled_duty <= 0.0f - motor_deadzone) || (last_enabled_duty >= motor_deadzone)) {
      duty = last_enabled_duty;
    }

    if(enabled)
      return duty;
    else
      return NAN;
  }

  float MotorState::set_duty_with_return(float duty) {
    // Invert provided speed if the motor direction is reversed
    if(motor_direction == REVERSED_DIR)
      duty = 0.0f - duty;

    // Clamp the duty between the hard limits
    last_enabled_duty = CLAMP(duty, -1.0f, 1.0f);

    // Calculate the corresponding speed
    motor_speed = duty_to_speed(last_enabled_duty, motor_zeropoint, motor_scale);

    return enable_with_return();
  }

  float MotorState::get_speed() const {
    return (motor_direction == NORMAL_DIR) ? motor_speed : 0.0f - motor_speed;
  }

  float MotorState::set_speed_with_return(float speed) {
    // Invert provided speed if the motor direction is reversed
    if(motor_direction == REVERSED_DIR)
      speed = 0.0f - speed;

    // Clamp the speed between the hard limits
    motor_speed = CLAMP(speed, 0.0f - motor_scale, motor_scale);

    // Calculate the corresponding duty cycle
    last_enabled_duty = speed_to_duty(motor_speed, motor_zeropoint, motor_scale);

    return enable_with_return();
  }

  float MotorState::stop_with_return() {
    return set_duty_with_return(0.0f);
  }

  float MotorState::full_negative_with_return() {
    return set_duty_with_return(-1.0f);
  }

  float MotorState::full_positive_with_return() {
    return set_duty_with_return(1.0f);
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max) {
    float speed = MotorState::map_float(in, in_min, in_max, 0.0f - motor_scale, motor_scale);
    return set_speed_with_return(speed);
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max) {
    float speed = MotorState::map_float(in, in_min, in_max, speed_min, speed_max);
    return set_speed_with_return(speed);
  }

  Direction MotorState::get_direction() const {
    return motor_direction;
  }

  void MotorState::set_direction(Direction direction) {
    motor_direction = direction;
  }

  float MotorState::get_speed_scale() const {
    return motor_scale;
  }

  void MotorState::set_speed_scale(float speed_scale) {
    motor_scale = MAX(speed_scale, FLT_EPSILON);
    motor_speed = duty_to_speed(last_enabled_duty, motor_zeropoint, motor_scale);
  }

  float MotorState::get_zeropoint() const {
    return motor_scale;
  }

  void MotorState::set_zeropoint(float zeropoint) {
    motor_zeropoint = CLAMP(zeropoint, 0.0f, 1.0f - FLT_EPSILON);
    motor_speed = duty_to_speed(last_enabled_duty, motor_zeropoint, motor_scale);
  }

  float MotorState::get_deadzone() const {
    return motor_deadzone;
  }

  float MotorState::set_deadzone_with_return(float deadzone) {
    motor_deadzone = CLAMP(deadzone, 0.0f, 1.0f);
    return get_deadzoned_duty();
  }

  int32_t MotorState::duty_to_level(float duty, uint32_t resolution) {
    return (int32_t)(duty * (float)resolution);
  }

  float MotorState::map_float(float in, float in_min, float in_max, float out_min, float out_max) {
    return (((in - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
  }

  float MotorState::duty_to_speed(float duty, float zeropoint, float scale) {
    float speed = 0.0f;
    if(duty > zeropoint) {
      speed = map_float(duty, zeropoint, 1.0f, 0.0f, scale);
    }
    else if(duty < -zeropoint) {
      speed = map_float(duty, -zeropoint, -1.0f, 0.0f, -scale);
    }
    return speed;
  }

  float MotorState::speed_to_duty(float speed, float zeropoint, float scale) {
    float duty = 0.0f;
    if(speed > 0.0f) {
      duty = map_float(speed, 0.0f, scale, zeropoint, 1.0f);
    }
    else if(speed < 0.0f) {
      duty = map_float(speed, 0.0f, -scale, -zeropoint, -1.0f);
    }
    return duty;
  }

};