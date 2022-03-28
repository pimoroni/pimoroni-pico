#include "motor_state.hpp"

namespace motor {
  MotorState::MotorState() {
  }

  //MotorState::MotorState(CalibrationType default_type)
  //  : calib(default_type) {
  //}

  //MotorState::MotorState(const Calibration& calibration) 
  //  : calib(calibration) {
  //}

  float MotorState::enable() {
    return _enable();
  }

  float MotorState::disable() {
    enabled = false;
    return 0.0f; // A zero duty
  }

  float MotorState::_enable() {
    enabled = true;
    return last_enabled_duty;
  }

  bool MotorState::is_enabled() const {
    return enabled;
  }

  float MotorState::get_duty() const {
    return last_enabled_duty;
  }

  float MotorState::set_duty_with_return(float duty) {
    //TODO
    // float speed_out, duty_out;
    // if(calib.duty_to_speed(duty, speed_out, duty_out)) {
    //   motor_speed = speed_out;
    //   last_enabled_duty = duty_out;
    //   return _enable();
    // }
    return disable();
  }

  float MotorState::get_speed() const {
    return motor_speed;
  }

  float MotorState::set_speed_with_return(float speed) {
    //TODO
    // float duty_out, speed_out;
    // if(calib.speed_to_duty(speed, duty_out, speed_out)) {
    //   last_enabled_duty = duty_out;
    //   motor_speed = speed_out;
    //   return _enable();
    // }
    return disable();
  }

  float MotorState::get_min_speed() const {
    float speed = 0.0f;
    //TODO
    //if(calib.size() > 0) {
    //  speed = calib.first().speed;
    //}
    return speed;
  }

  // float MotorState::get_mid_speed() const {
  //   float speed = 0.0f;
  //   if(calib.size() > 0) {
  //     const Calibration::Pair &first = calib.first();
  //     const Calibration::Pair &last = calib.last();
  //     speed = (first.speed + last.speed) / 2.0f;
  //   }
  //   return speed;
  // }

  float MotorState::get_max_speed() const {
    float speed = 0.0f;
    //TODO
    //if(calib.size() > 0) {
    //  speed = calib.last().speed;
    //}
    return speed;
  }

  float MotorState::to_min_with_return() {
    return set_speed_with_return(get_min_speed());
  }

  // float MotorState::to_mid_with_return() {
  //   return set_speed_with_return(get_mid_speed());
  // }

  float MotorState::to_max_with_return() {
    return set_speed_with_return(get_max_speed());
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max) {
    float speed = MotorState::map_float(in, in_min, in_max, get_min_speed(), get_max_speed());
    return set_speed_with_return(speed);
  }

  float MotorState::to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max) {
    float speed = MotorState::map_float(in, in_min, in_max, speed_min, speed_max);
    return set_speed_with_return(speed);
  }

  // Calibration& MotorState::calibration() {
  //   return calib;
  // }

  // const Calibration& MotorState::calibration() const {
  //   return calib;
  // }

  int32_t MotorState::duty_to_level(float duty, uint32_t resolution) {
    return (int32_t)(duty * (float)resolution);
  }

  float MotorState::map_float(float in, float in_min, float in_max, float out_min, float out_max) {
    return (((in - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
  }

};