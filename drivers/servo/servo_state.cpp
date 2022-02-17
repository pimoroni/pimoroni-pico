#include "servo_state.hpp"

namespace servo {
  ServoState::ServoState(/*uint pin, */Type type)
    : /*pin(pin), */converter(type) {
  }

  float ServoState::enable() {
    if(last_enabled_pulse < Converter::MIN_VALID_PULSE) {
      servo_value = converter.get_mid_value();
      last_enabled_pulse = converter.value_to_pulse(servo_value);
    }
    enabled = true;
    return last_enabled_pulse;
  }

  float ServoState::disable() {
    enabled = false;
    return 0.0f; // A zero pulse
  }

  bool ServoState::is_enabled() {
    return enabled;
  }

  float ServoState::get_value() {
    return servo_value;
  }

  float ServoState::set_value(float value) {
    servo_value = value;
    float pulse = converter.value_to_pulse(value);
    if(pulse >= Converter::MIN_VALID_PULSE) {
      last_enabled_pulse = pulse;
      enabled = true;
    }
    else {
      pulse = disable();
    }
    return pulse;
  }

  float ServoState::get_pulse() {
    return last_enabled_pulse;
  }

  float ServoState::set_pulse(float pulse) {
    if(pulse >= Converter::MIN_VALID_PULSE) {
      servo_value = converter.value_from_pulse(pulse);
      last_enabled_pulse = pulse;
      enabled = true;
    }
    else {
      pulse = disable();
    }
    return pulse;
  }

  float ServoState::to_min() {
    return set_value(converter.get_min_value());
  }

  float ServoState::to_mid() {
    return set_value(converter.get_mid_value());
  }

  float ServoState::to_max() {
    return set_value(converter.get_max_value());
  }

  float ServoState::to_percent(float in, float in_min, float in_max) {
    float value = Converter::map_float(in, in_min, in_max, converter.get_min_value(), converter.get_max_value());
    return set_value(value);
  }

  float ServoState::to_percent(float in, float in_min, float in_max, float value_min, float value_max) {
    float value = Converter::map_float(in, in_min, in_max, value_min, value_max);
    return set_value(value);
  }

  float ServoState::get_min_value() {
    return converter.get_min_value();
  }

  float ServoState::get_mid_value() {
    return converter.get_mid_value();
  }

  float ServoState::get_max_value() {
    return converter.get_max_value();
  }

  Calibration& ServoState::calibration() {
    return converter;
  }
};