#include "servo.hpp"
#include "hardware/clocks.h"
#include "pwm.hpp"

namespace servo {
  Servo::Servo(uint pin, CalibrationType type, float freq)
    : servo_pin(pin), state(type), pwm_frequency(freq) {
  }

  Servo::Servo(uint pin, const Calibration& calibration, float freq)
    : servo_pin(pin), state(calibration), pwm_frequency(freq) {
  }

  Servo::~Servo() {
    gpio_set_function(servo_pin, GPIO_FUNC_NULL);
  }

  bool Servo::init() {
    bool success = false;

    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, pwm_period - 1);

      // Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

      pwm_init(pwm_gpio_to_slice_num(servo_pin), &pwm_cfg, true);
      gpio_set_function(servo_pin, GPIO_FUNC_PWM);

      pwm_set_gpio_level(servo_pin, 0);

      success = true;
    }
    return success;
  }

  uint Servo::pin() const {
    return servo_pin;
  }

  void Servo::enable() {
    apply_pulse(state.enable_with_return());
  }

  void Servo::disable() {
    apply_pulse(state.disable_with_return());
  }

  bool Servo::is_enabled() const {
    return state.is_enabled();
  }

  float Servo::pulse() const {
    return state.get_pulse();
  }

  void Servo::pulse(float pulse) {
    apply_pulse(state.set_pulse_with_return(pulse));
  }

  float Servo::value() const {
    return state.get_value();
  }

  void Servo::value(float value) {
    apply_pulse(state.set_value_with_return(value));
  }

  float Servo::frequency() const {
    return pwm_frequency;
  }

  bool Servo::frequency(float freq) {
    bool success = false;

    if((freq >= ServoState::MIN_FREQUENCY) && (freq <= ServoState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint16_t period; uint16_t div16;
      if(pimoroni::calculate_pwm_factors(freq, period, div16)) {

        // Record if the new period will be larger or smaller.
        // This is used to apply new pwm values either before or after the wrap is applied,
        // to avoid momentary blips in PWM output on SLOW_DECAY
        bool pre_update_pwm = (period > pwm_period);

        pwm_period = period;
        pwm_frequency = freq;

        uint pin_num = pwm_gpio_to_slice_num(servo_pin);

        // Apply the new divider
        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
        pwm_set_clkdiv_int_frac(pin_num, div, mod);

        // If the the period is larger, update the pwm before setting the new wraps
        if(state.is_enabled() && pre_update_pwm) {
          apply_pulse(state.get_pulse());
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwm_set_wrap(pin_num, pwm_period - 1);

        // If the the period is smaller, update the pwm after setting the new wraps
        if(state.is_enabled() && !pre_update_pwm) {
          apply_pulse(state.get_pulse());
        }

        success = true;
      }
    }
    return success;
  }

  float Servo::min_value() const {
    return state.get_min_value();
  }

  float Servo::mid_value() const {
    return state.get_mid_value();
  }

  float Servo::max_value() const {
    return state.get_max_value();
  }

  void Servo::to_min() {
    apply_pulse(state.to_min_with_return());
  }

  void Servo::to_mid() {
    apply_pulse(state.to_mid_with_return());
  }

  void Servo::to_max() {
    apply_pulse(state.to_max_with_return());
  }

  void Servo::to_percent(float in, float in_min, float in_max) {
    apply_pulse(state.to_percent_with_return(in, in_min, in_max));
  }

  void Servo::to_percent(float in, float in_min, float in_max, float value_min, float value_max) {
    apply_pulse(state.to_percent_with_return(in, in_min, in_max, value_min, value_max));
  }

  Calibration& Servo::calibration() {
    return state.calibration();
  }

  const Calibration& Servo::calibration() const {
    return state.calibration();
  }

  void Servo::apply_pulse(float pulse) {
    pwm_set_gpio_level(servo_pin, (uint16_t)ServoState::pulse_to_level(pulse, pwm_period, pwm_frequency));
  }
};