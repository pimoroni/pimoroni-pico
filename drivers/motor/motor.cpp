#include "motor.hpp"
#include "hardware/clocks.h"
#include "pwm.hpp"
#include "math.h"

namespace motor {
  Motor::Driver::Driver(const pin_pair &pins) : motor_pins(pins), pwm_period(1) {
  }

  Motor::Driver::~Driver() {
    gpio_set_function(motor_pins.first, GPIO_FUNC_NULL);
    gpio_set_function(motor_pins.second, GPIO_FUNC_NULL);
  }

  const pin_pair& Motor::Driver::pins() const {
    return motor_pins;
  }

  void Motor::DualPWMDriver::init(pwm_config *pwm_cfg, uint16_t period) {
    pwm_period = period;

    // Set up the positive and negative pins
    pwm_init(pwm_gpio_to_slice_num(motor_pins.positive), pwm_cfg, true);
    pwm_init(pwm_gpio_to_slice_num(motor_pins.negative), pwm_cfg, true);

    gpio_set_function(motor_pins.positive, GPIO_FUNC_PWM);
    gpio_set_function(motor_pins.negative, GPIO_FUNC_PWM);

    pwm_set_gpio_level(motor_pins.positive, 0);
    pwm_set_gpio_level(motor_pins.negative, 0);
  }

  void Motor::DualPWMDriver::update_frequency(uint8_t div, uint8_t mod, uint16_t period, float duty, DecayMode mode) {
    // Record if the new period will be larger or smaller.
    // This is used to apply new pwm speeds either before or after the wrap is applied,
    // to avoid momentary blips in PWM output on SLOW_DECAY
    bool pre_update_pwm = (period > pwm_period);
    pwm_period = period;

    uint pos_pin_slice = pwm_gpio_to_slice_num(motor_pins.positive);
    uint neg_pin_slice = pwm_gpio_to_slice_num(motor_pins.negative);

    // Apply the new divider
    pwm_set_clkdiv_int_frac(pos_pin_slice, div, mod);
    if((neg_pin_slice != pos_pin_slice))
      pwm_set_clkdiv_int_frac(neg_pin_slice, div, mod);

    // If the period is larger, update the pwm before setting the new wraps
    if(pre_update_pwm) {
      apply_duty(duty, mode);
    }

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_set_wrap(pos_pin_slice, pwm_period - 1);
    if(neg_pin_slice != pos_pin_slice)
      pwm_set_wrap(neg_pin_slice, pwm_period - 1);

    // If the period is smaller, update the pwm after setting the new wraps
    if(!pre_update_pwm) {
      apply_duty(duty, mode);
    }
  }

  void Motor::DualPWMDriver::apply_duty(float duty, DecayMode mode) {
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

  void Motor::PhEnDriver::init(pwm_config *pwm_cfg, uint16_t period) {
    pwm_period = period;

    // Set up the phase and enable pins
    gpio_init(motor_pins.phase);
    pwm_init(pwm_gpio_to_slice_num(motor_pins.enable), pwm_cfg, true);

    gpio_set_dir(motor_pins.phase, true);
    gpio_set_function(motor_pins.enable, GPIO_FUNC_PWM);

    gpio_put(motor_pins.phase, false);
    pwm_set_gpio_level(motor_pins.enable, 0);
  }


  void Motor::PhEnDriver::update_frequency(uint8_t div, uint8_t mod, uint16_t period, float duty, DecayMode mode) {
    // Record if the new period will be larger or smaller.
    // This is used to apply new pwm speeds either before or after the wrap is applied,
    // to avoid momentary blips in PWM output on SLOW_DECAY
    bool pre_update_pwm = (period > pwm_period);
    pwm_period = period;

    uint en_pin_slice = pwm_gpio_to_slice_num(motor_pins.enable);

    // Apply the new divider
    pwm_set_clkdiv_int_frac(en_pin_slice, div, mod);

    // If the period is larger, update the pwm before setting the new wraps
    if(pre_update_pwm) {
      apply_duty(duty, mode);
    }

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_set_wrap(en_pin_slice, pwm_period - 1);

    // If the period is smaller, update the pwm after setting the new wraps
    if(!pre_update_pwm) {
      apply_duty(duty, mode);
    }
  }

  void Motor::PhEnDriver::apply_duty(float duty, DecayMode mode) {
    if(isfinite(duty)) {
      int32_t signed_level = MotorState::duty_to_level(duty, pwm_period);

      if(signed_level >= 0) {
        gpio_put(motor_pins.phase, true);
        pwm_set_gpio_level(motor_pins.enable, signed_level);
      }
      else {
        gpio_put(motor_pins.phase, false);
        pwm_set_gpio_level(motor_pins.enable, 0 - signed_level);
      }
    }
    else {
      gpio_put(motor_pins.phase, false);
      pwm_set_gpio_level(motor_pins.enable, 0);
    }
  }

  Motor::Motor(const pin_pair &pins, Direction direction, float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode, bool ph_en_driver)
  : driver(ph_en_driver ? (Driver*)new PhEnDriver(pins) : (Driver*)new DualPWMDriver(pins))
  , state(direction, speed_scale, zeropoint, deadzone), pwm_frequency(freq), motor_mode(mode) {
  }

  Motor::~Motor() {
    delete driver;
  }

  bool Motor::init() {
    bool success = false;

    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_cfg = pwm_get_default_config();

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, period - 1);

      // Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

      // Set up the pin driver
      driver->init(&pwm_cfg, period);

      success = true;
    }
    return success;
  }

  pin_pair Motor::pins() const {
    return driver->pins();
  }

  void Motor::enable() {
    driver->apply_duty(state.enable_with_return(), motor_mode);
  }

  void Motor::disable() {
    driver->apply_duty(state.disable_with_return(), motor_mode);
  }

  bool Motor::is_enabled() const {
    return state.is_enabled();
  }

  float Motor::duty() const {
    return state.get_duty();
  }

  void Motor::duty(float duty) {
    driver->apply_duty(state.set_duty_with_return(duty), motor_mode);
  }

  float Motor::speed() const {
    return state.get_speed();
  }

  void Motor::speed(float speed) {
    driver->apply_duty(state.set_speed_with_return(speed), motor_mode);
  }

  float Motor::frequency() const {
    return pwm_frequency;
  }

  bool Motor::frequency(float freq) {
    bool success = false;

    if((freq >= MotorState::MIN_FREQUENCY) && (freq <= MotorState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint16_t period; uint16_t div16;
      if(pimoroni::calculate_pwm_factors(freq, period, div16)) {

        pwm_frequency = freq;

        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
        driver->update_frequency(div, mod, period, state.get_deadzoned_duty(), motor_mode);

        success = true;
      }
    }
    return success;
  }

  void Motor::stop() {
    driver->apply_duty(state.stop_with_return(), motor_mode);
  }

  void Motor::coast() {
    driver->apply_duty(state.stop_with_return(), FAST_DECAY);
  }

  void Motor::brake() {
    driver->apply_duty(state.stop_with_return(), SLOW_DECAY);
  }

  void Motor::full_negative() {
    driver->apply_duty(state.full_negative_with_return(), motor_mode);
  }

  void Motor::full_positive() {
    driver->apply_duty(state.full_positive_with_return(), motor_mode);
  }

  void Motor::to_percent(float in, float in_min, float in_max) {
    driver->apply_duty(state.to_percent_with_return(in, in_min, in_max), motor_mode);
  }

  void Motor::to_percent(float in, float in_min, float in_max, float speed_min, float speed_max) {
    driver->apply_duty(state.to_percent_with_return(in, in_min, in_max, speed_min, speed_max), motor_mode);
  }

  Direction Motor::direction() const {
    return state.get_direction();
  }

  void Motor::direction(Direction direction) {
    state.set_direction(direction);
  }

  float Motor::speed_scale() const {
    return state.get_speed_scale();
  }

  void Motor::speed_scale(float speed_scale) {
    state.set_speed_scale(speed_scale);
  }

  float Motor::zeropoint() const {
    return state.get_zeropoint();
  }

  void Motor::zeropoint(float zeropoint) {
    state.set_zeropoint(zeropoint);
  }

  float Motor::deadzone() const {
    return state.get_deadzone();
  }

  void Motor::deadzone(float deadzone) {
    driver->apply_duty(state.set_deadzone_with_return(deadzone), motor_mode);
  }

  DecayMode Motor::decay_mode() {
    return motor_mode;
  }

  void Motor::decay_mode(DecayMode mode) {
    motor_mode = mode;
    driver->apply_duty(state.get_deadzoned_duty(), motor_mode);
  }
};