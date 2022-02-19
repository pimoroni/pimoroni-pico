#include "servo.hpp"
#include "hardware/clocks.h"

namespace servo {
  Servo::Servo(uint pin, CalibrationType type)
    : pin(pin), state(type) {
  }

  Servo::~Servo() {
    gpio_set_function(pin, GPIO_FUNC_NULL);
  }

  bool Servo::init() {
    bool success = false;

    uint16_t period; uint16_t div16;
    if(Servo::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, pwm_period - 1);

      // Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

      pwm_init(pwm_gpio_to_slice_num(pin), &pwm_cfg, true);
      gpio_set_function(pin, GPIO_FUNC_PWM);

      pwm_set_gpio_level(pin, 0);

      success = true;
    }
    return success;
  }

  uint Servo::get_pin() const {
    return pin;
  }

  void Servo::enable() {
    float new_pulse = state.enable();
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  void Servo::disable() {
    float new_pulse = state.disable();
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  bool Servo::is_enabled() const {
    return state.is_enabled();
  }

  float Servo::get_value() const {
    return state.get_value();
  }

  void Servo::set_value(float value) {
    float new_pulse = state.set_value(value);
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  float Servo::get_pulse() const {
    return state.get_pulse();
  }

  void Servo::set_pulse(float pulse) {
    float new_pulse = state.set_pulse(pulse);
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }


  float Servo::get_frequency() const {
    return pwm_frequency;
  }

  bool Servo::set_frequency(float freq) {
    bool success = false;

    // Calculate a suitable pwm wrap period for this frequency
    uint16_t period; uint16_t div16;
    if(Servo::calculate_pwm_factors(freq, period, div16)) {

      // Record if the new period will be larger or smaller.
      // This is used to apply new pwm values either before or after the wrap is applied,
      // to avoid momentary blips in PWM output on SLOW_DECAY
      bool pre_update_pwm = (period > pwm_period);

      pwm_period = period;
      pwm_frequency = freq;

      uint pin_num = pwm_gpio_to_slice_num(pin);

      // Apply the new divider
      uint8_t div = div16 >> 4;
      uint8_t mod = div16 % 16;
      pwm_set_clkdiv_int_frac(pin_num, div, mod);

      // If the the period is larger, update the pwm before setting the new wraps
      if(pre_update_pwm) {
        float current_pulse = get_pulse();
        pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(current_pulse, pwm_period, pwm_frequency));
      }

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_set_wrap(pin_num, pwm_period - 1);

      // If the the period is smaller, update the pwm after setting the new wraps
      if(!pre_update_pwm) {
        float current_pulse = get_pulse();
        pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(current_pulse, pwm_period, pwm_frequency));
      }

      success = true;
    }
    return success;
  }

  float Servo::get_min_value() const {
    return state.get_min_value();
  }

  float Servo::get_mid_value() const {
    return state.get_mid_value();
  }

  float Servo::get_max_value() const {
    return state.get_max_value();
  }

  void Servo::to_min() {
    float new_pulse = state.to_min();
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  void Servo::to_mid() {
    float new_pulse = state.to_mid();
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  void Servo::to_max() {
    float new_pulse = state.to_max();
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  void Servo::to_percent(float in, float in_min, float in_max) {
    float new_pulse = state.to_percent(in, in_min, in_max);
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  void Servo::to_percent(float in, float in_min, float in_max, float value_min, float value_max) {
    float new_pulse = state.to_percent(in, in_min, in_max, value_min, value_max);
    pwm_set_gpio_level(pin, (uint16_t)ServoState::pulse_to_level(new_pulse, pwm_period, pwm_frequency));
  }

  Calibration& Servo::calibration() {
    return state.calibration();
  }

  const Calibration& Servo::calibration() const {
    return state.calibration();
  }

  // Derived from the rp2 Micropython implementation: https://github.com/micropython/micropython/blob/master/ports/rp2/machine_pwm.c
  bool Servo::calculate_pwm_factors(float freq, uint16_t& top_out, uint16_t& div16_out) {
    bool success = false;
    uint32_t source_hz = clock_get_hz(clk_sys);

    // Check the provided frequency is valid
    if((freq >= 1.0f) && (freq <= (float)(source_hz >> 1))) {
      uint32_t div16_top = (uint32_t)((float)(source_hz << 4) / freq);
      uint32_t top = 1;

      while(true) {
          // Try a few small prime factors to get close to the desired frequency.
          if((div16_top >= (5 << 4)) && (div16_top % 5 == 0) && (top * 5 <= MAX_PWM_WRAP)) {
              div16_top /= 5;
              top *= 5;
          }
          else if((div16_top >= (3 << 4)) && (div16_top % 3 == 0) && (top * 3 <= MAX_PWM_WRAP)) {
              div16_top /= 3;
              top *= 3;
          }
          else if((div16_top >= (2 << 4)) && (top * 2 <= MAX_PWM_WRAP)) {
              div16_top /= 2;
              top *= 2;
          }
          else {
              break;
          }
      }
      if(div16_top >= 16 && div16_top <= (UINT8_MAX << 4)) {
        top_out = top;
        div16_out = div16_top;

        success = true;
      }
    }
    return success;
  }

};