#include "servo_state.hpp"

namespace servo {
  ServoState::ServoState()
    : servo_value(0.0f), last_enabled_pulse(0.0f), enabled(false) {
  }

  ServoState::ServoState(CalibrationType default_type)
    : servo_value(0.0f), last_enabled_pulse(0.0f), enabled(false), calib(default_type) {
  }

  ServoState::ServoState(const Calibration& calibration) 
    : servo_value(0.0f), last_enabled_pulse(0.0f), enabled(false), calib(calibration) {
  }

  float ServoState::enable_with_return() {
    // Has the servo not had a pulse value set before being enabled?
    if(last_enabled_pulse < MIN_VALID_PULSE) {
      // Set the servo to its middle
      return to_mid_with_return();
    }
    return _enable_with_return();
  }

  float ServoState::disable_with_return() {
    enabled = false;
    return 0.0f; // A zero pulse
  }

  float ServoState::_enable_with_return() {
    enabled = true;
    return last_enabled_pulse;
  }

  bool ServoState::is_enabled() const {
    return enabled;
  }

  float ServoState::get_pulse() const {
    return last_enabled_pulse;
  }

  float ServoState::set_pulse_with_return(float pulse) {
    if(pulse >= MIN_VALID_PULSE) {
      float value_out, pulse_out;
      if(calib.pulse_to_value(pulse, value_out, pulse_out)) {
        servo_value = value_out;
        last_enabled_pulse = pulse_out;
        return _enable_with_return();
      }
    }
    return disable_with_return();
  }

  float ServoState::get_value() const {
    return servo_value;
  }

  float ServoState::set_value_with_return(float value) {
    float pulse_out, value_out;
    if(calib.value_to_pulse(value, pulse_out, value_out)) {
      last_enabled_pulse = pulse_out;
      servo_value = value_out;
      return _enable_with_return();
    }
    return disable_with_return();
  }

  float ServoState::get_min_value() const {
    float value = 0.0f;
    if(calib.size() > 0) {
      value = calib.first().value;
    }
    return value;
  }

  float ServoState::get_mid_value() const {
    float value = 0.0f;
    if(calib.size() > 0) {
      const Calibration::Pair &first = calib.first();
      const Calibration::Pair &last = calib.last();
      value = (first.value + last.value) / 2.0f;
    }
    return value;
  }

  float ServoState::get_max_value() const {
    float value = 0.0f;
    if(calib.size() > 0) {
      value = calib.last().value;
    }
    return value;
  }

  float ServoState::to_min_with_return() {
    return set_value_with_return(get_min_value());
  }

  float ServoState::to_mid_with_return() {
    return set_value_with_return(get_mid_value());
  }

  float ServoState::to_max_with_return() {
    return set_value_with_return(get_max_value());
  }

  float ServoState::to_percent_with_return(float in, float in_min, float in_max) {
    float value = Calibration::map_float(in, in_min, in_max, get_min_value(), get_max_value());
    return set_value_with_return(value);
  }

  float ServoState::to_percent_with_return(float in, float in_min, float in_max, float value_min, float value_max) {
    float value = Calibration::map_float(in, in_min, in_max, value_min, value_max);
    return set_value_with_return(value);
  }

  Calibration& ServoState::calibration() {
    return calib;
  }

  const Calibration& ServoState::calibration() const {
    return calib;
  }

  uint32_t ServoState::pulse_to_level(float pulse, uint32_t resolution, float freq) {
    uint32_t level = 0;
    if(pulse >= MIN_VALID_PULSE) {
        level = (uint32_t)((pulse * (float)resolution * freq) / 1000000);
    }
    return level;
  }

};