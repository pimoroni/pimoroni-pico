#include "motor2.hpp"
#include "hardware/clocks.h"
#include "pwm.hpp"
#include "math.h"

namespace motor {
  Motor2::Motor2(const pin_pair &pins, Direction direction, float speed_scale, float deadzone, float freq, DecayMode mode)
    : motor_pins(pins), state(direction, speed_scale, deadzone), pwm_frequency(freq), motor_mode(mode) {
  }

  Motor2::~Motor2() {
    gpio_set_function(motor_pins.positive, GPIO_FUNC_NULL);
    gpio_set_function(motor_pins.negative, GPIO_FUNC_NULL);
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

      pwm_init(pwm_gpio_to_slice_num(motor_pins.positive), &pwm_cfg, true);
      pwm_init(pwm_gpio_to_slice_num(motor_pins.negative), &pwm_cfg, true);

      gpio_set_function(motor_pins.positive, GPIO_FUNC_PWM);
      gpio_set_function(motor_pins.negative, GPIO_FUNC_PWM);

      pwm_set_gpio_level(motor_pins.positive, 0);
      pwm_set_gpio_level(motor_pins.negative, 0);

      success = true;
    }
    return success;
  }

  pin_pair Motor2::pins() const {
    return motor_pins;
  }

  void Motor2::enable() {
    apply_duty(state.enable_with_return(), motor_mode);
  }

  void Motor2::disable() {
    apply_duty(state.disable_with_return(), motor_mode);
  }

  bool Motor2::is_enabled() const {
    return state.is_enabled();
  }

  float Motor2::duty() const {
    return state.get_duty();
  }

  void Motor2::duty(float duty) {
    apply_duty(state.set_duty_with_return(duty), motor_mode);
  }

  float Motor2::speed() const {
    return state.get_speed();
  }

  void Motor2::speed(float speed) {
    apply_duty(state.set_speed_with_return(speed), motor_mode);
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

        uint pos_pin_num = pwm_gpio_to_slice_num(motor_pins.positive);
        uint neg_pin_num = pwm_gpio_to_slice_num(motor_pins.negative);

        // Apply the new divider
        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
        pwm_set_clkdiv_int_frac(pos_pin_num, div, mod);
        if(neg_pin_num != pos_pin_num)
          pwm_set_clkdiv_int_frac(neg_pin_num, div, mod);

        // If the period is larger, update the pwm before setting the new wraps
        if(pre_update_pwm) {
          apply_duty(state.get_deadzoned_duty(), motor_mode);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwm_set_wrap(pos_pin_num, pwm_period - 1);
        if(neg_pin_num != pos_pin_num)
          pwm_set_wrap(neg_pin_num, pwm_period - 1);

        // If the period is smaller, update the pwm after setting the new wraps
        if(!pre_update_pwm) {
          apply_duty(state.get_deadzoned_duty(), motor_mode);
        }

        success = true;
      }
    }
    return success;
  }

  void Motor2::stop() {
    apply_duty(state.stop_with_return(), motor_mode);
  }

  void Motor2::coast() {
    apply_duty(state.stop_with_return(), FAST_DECAY);
  }

  void Motor2::brake() {
    apply_duty(state.stop_with_return(), SLOW_DECAY);
  }

  void Motor2::full_negative() {
    apply_duty(state.full_negative_with_return(), motor_mode);
  }

  void Motor2::full_positive() {
    apply_duty(state.full_positive_with_return(), motor_mode);
  }

  void Motor2::to_percent(float in, float in_min, float in_max) {
    apply_duty(state.to_percent_with_return(in, in_min, in_max), motor_mode);
  }

  void Motor2::to_percent(float in, float in_min, float in_max, float speed_min, float speed_max) {
    apply_duty(state.to_percent_with_return(in, in_min, in_max, speed_min, speed_max), motor_mode);
  }

  Direction Motor2::direction() const {
    return state.get_direction();
  }

  void Motor2::direction(Direction direction) {
    state.set_direction(direction);
  }

  float Motor2::speed_scale() const {
    return state.get_speed_scale();
  }

  void Motor2::speed_scale(float speed_scale) {
    state.set_speed_scale(speed_scale);
  }

  float Motor2::deadzone() const {
    return state.get_deadzone();
  }

  void Motor2::deadzone(float deadzone) {
    apply_duty(state.set_deadzone_with_return(deadzone), motor_mode);
  }

  DecayMode Motor2::decay_mode() {
    return motor_mode;
  }

  void Motor2::decay_mode(DecayMode mode) {
    motor_mode = mode;
    apply_duty(state.get_deadzoned_duty(), motor_mode);
  }

  void Motor2::apply_duty(float duty, DecayMode mode) {
    if(isfinite(duty)) {
      int32_t signed_level = MotorState::duty_to_level(duty, pwm_period);

      switch(mode) {
      case SLOW_DECAY: //aka 'Braking'
        if(signed_level >= 0) {
          pwm_set_gpio_level(motor_pins.positive, pwm_period);
          pwm_set_gpio_level(motor_pins.negative, pwm_period - signed_level);
        }
        else {
          pwm_set_gpio_level(motor_pins.positive, pwm_period + signed_level);
          pwm_set_gpio_level(motor_pins.negative, pwm_period);
        }
        break;

      case FAST_DECAY: //aka 'Coasting'
      default:
        if(signed_level >= 0) {
          pwm_set_gpio_level(motor_pins.positive, signed_level);
          pwm_set_gpio_level(motor_pins.negative, 0);
        }
        else {
          pwm_set_gpio_level(motor_pins.positive, 0);
          pwm_set_gpio_level(motor_pins.negative, 0 - signed_level);
        }
        break;
      }
    }
    else {
      pwm_set_gpio_level(motor_pins.positive, 0);
      pwm_set_gpio_level(motor_pins.negative, 0);
    }
  }
};