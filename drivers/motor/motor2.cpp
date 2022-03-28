#include "motor2.hpp"
#include "hardware/clocks.h"
#include "pwm.hpp"

namespace motor {
  Motor2::Motor2(uint pin_pos, uint pin_neg, float freq, MotorState::DecayMode mode)
    : motor_pin_pos(pin_pos), motor_pin_neg(pin_neg), pwm_frequency(freq), motor_decay_mode(mode) {
  }

  // Motor2::Motor2(uint pin, /*const Calibration& calibration,*/ float freq)
  //   : motor_pin_pos(pin), /*state(calibration),*/ pwm_frequency(freq) {
  // }

  Motor2::~Motor2() {
    gpio_set_function(motor_pin_pos, GPIO_FUNC_NULL);
    gpio_set_function(motor_pin_neg, GPIO_FUNC_NULL);
  }

  bool Motor2::init() {
    bool success = false;

    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, pwm_period - 1);

      // Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

      pwm_init(pwm_gpio_to_slice_num(motor_pin_pos), &pwm_cfg, true);
      gpio_set_function(motor_pin_pos, GPIO_FUNC_PWM);

      pwm_init(pwm_gpio_to_slice_num(motor_pin_neg), &pwm_cfg, true);
      gpio_set_function(motor_pin_neg, GPIO_FUNC_PWM);

      pwm_set_gpio_level(motor_pin_pos, 0);
      pwm_set_gpio_level(motor_pin_neg, 0);

      success = true;
    }
    return success;
  }

  uint Motor2::pin() const {
    return motor_pin_pos;
  }

  void Motor2::enable() {
    apply_duty(state.enable());
  }

  void Motor2::disable() {
    apply_duty(state.disable());
  }

  bool Motor2::is_enabled() const {
    return state.is_enabled();
  }

  // float Motor2::duty() const {
  //   return state.get_duty();
  // }

  // void Motor2::duty(float duty) {
  //   apply_duty(state.set_duty_with_return(duty));
  // }

  float Motor2::speed() const {
    return state.get_speed();
  }

  void Motor2::speed(float speed) {
    apply_duty(state.set_speed_with_return(speed));
  }

  float Motor2::frequency() const {
    return pwm_frequency;
  }

  bool Motor2::frequency(float freq) {
    bool success = false;

    if((freq >= MotorState::MIN_FREQUENCY) && (freq <= MotorState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint16_t period; uint16_t div16;
      if(pimoroni::calculate_pwm_factors(freq, period, div16)) {

        // Record if the new period will be larger or smaller.
        // This is used to apply new pwm speeds either before or after the wrap is applied,
        // to avoid momentary blips in PWM output on SLOW_DECAY
        bool pre_update_pwm = (period > pwm_period);

        pwm_period = period;
        pwm_frequency = freq;

        uint pin_num = pwm_gpio_to_slice_num(motor_pin_pos);

        // Apply the new divider
        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
        pwm_set_clkdiv_int_frac(pin_num, div, mod);

        // If the the period is larger, update the pwm before setting the new wraps
        if(state.is_enabled() && pre_update_pwm) {
          apply_duty(state.get_duty());
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwm_set_wrap(pin_num, pwm_period - 1);

        // If the the period is smaller, update the pwm after setting the new wraps
        if(state.is_enabled() && !pre_update_pwm) {
          apply_duty(state.get_duty());
        }

        success = true;
      }
    }
    return success;
  }

  MotorState::DecayMode Motor2::decay_mode() {
    return motor_decay_mode;
  }

  void Motor2::decay_mode(MotorState::DecayMode mode) {
    motor_decay_mode = mode;
    apply_duty(state.get_duty());
  }

  void Motor2::stop() {
    apply_duty(state.set_speed_with_return(0.0f));
  }

  void Motor2::coast() {
    disable();
  }

  float Motor2::min_speed() const {
    return state.get_min_speed();
  }

  // float Motor2::mid_speed() const {
  //   return state.get_mid_speed();
  // }

  float Motor2::max_speed() const {
    return state.get_max_speed();
  }

  void Motor2::to_min() {
    apply_duty(state.to_min_with_return());
  }

  // void Motor2::to_mid() {
  //   apply_duty(state.to_mid_with_return());
  // }

  void Motor2::to_max() {
    apply_duty(state.to_max_with_return());
  }

  void Motor2::to_percent(float in, float in_min, float in_max) {
    apply_duty(state.to_percent_with_return(in, in_min, in_max));
  }

  void Motor2::to_percent(float in, float in_min, float in_max, float speed_min, float speed_max) {
    apply_duty(state.to_percent_with_return(in, in_min, in_max, speed_min, speed_max));
  }

  // Calibration& Motor2::calibration() {
  //   return state.calibration();
  // }

  // const Calibration& Motor2::calibration() const {
  //   return state.calibration();
  // }

  void Motor2::apply_duty(float duty) {
    int32_t signed_level = MotorState::duty_to_level(duty, pwm_period);

    switch(motor_decay_mode) {
    case MotorState::SLOW_DECAY: //aka 'Braking'
      if(signed_level >= 0) {
        pwm_set_gpio_level(motor_pin_pos, pwm_period);
        pwm_set_gpio_level(motor_pin_neg, pwm_period - signed_level);
      }
      else {
        pwm_set_gpio_level(motor_pin_pos, pwm_period + signed_level);
        pwm_set_gpio_level(motor_pin_neg, pwm_period);
      }
      break;

    case MotorState::FAST_DECAY: //aka 'Coasting'
    default:
      if(signed_level >= 0) {
        pwm_set_gpio_level(motor_pin_pos, signed_level);
        pwm_set_gpio_level(motor_pin_neg, 0);
      }
      else {
        pwm_set_gpio_level(motor_pin_pos, 0);
        pwm_set_gpio_level(motor_pin_neg, 0 - signed_level);
      }
      break;
    }
  }
};