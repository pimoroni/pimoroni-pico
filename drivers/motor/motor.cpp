#include "motor.hpp"
#include "hardware/clocks.h"
#include <math.h>

namespace pimoroni {
  Motor::Motor(uint pin_plus, uint pin_minus, float freq, DecayMode mode)
    : pin_plus(pin_plus), pin_minus(pin_minus), motor_mode(mode) {

    uint16_t period = 0;
    uint8_t divider = 1;
    Motor::calculate_pwm_period(freq, period, divider);
    pwm_period = period;

    pwm_cfg = pwm_get_default_config();
    pwm_config_set_wrap(&pwm_cfg, period);
    pwm_config_set_clkdiv_int(&pwm_cfg, divider);

    pwm_init(pwm_gpio_to_slice_num(pin_plus), &pwm_cfg, true);
    gpio_set_function(pin_plus, GPIO_FUNC_PWM);

    pwm_init(pwm_gpio_to_slice_num(pin_minus), &pwm_cfg, true);
    gpio_set_function(pin_minus, GPIO_FUNC_PWM);
    update_pwm();
  }

  Motor::~Motor() {
    gpio_set_function(pin_plus, GPIO_FUNC_NULL);
    gpio_set_function(pin_minus, GPIO_FUNC_NULL);
  }

  void Motor::set_speed(float speed) {
    motor_speed = speed;
    update_pwm();
  }

  void Motor::set_frequency(float freq) {
    //Calculate a suitable pwm wrap period for this frequency
    uint16_t period; uint8_t divider;
    if(Motor::calculate_pwm_period(freq, period, divider)) {
      pwm_period = period;

      //Stop PWM to the motor
      pwm_set_gpio_level(pin_plus, 0);
      pwm_set_gpio_level(pin_minus, 0);

      //Apply the new divider
      pwm_set_clkdiv_int_frac(pwm_gpio_to_slice_num(pin_plus), divider, 0);
      pwm_set_clkdiv_int_frac(pwm_gpio_to_slice_num(pin_minus), divider, 0);

      //Set the new wrap periods
      pwm_set_wrap(pwm_gpio_to_slice_num(pin_plus), pwm_period);
      pwm_set_wrap(pwm_gpio_to_slice_num(pin_minus), pwm_period);

      //Set the motor back into the state it was before
      update_pwm();
    }
  }

  Motor::DecayMode Motor::get_decay_mode() {
    return motor_mode;
  }

  void Motor::set_mode(Motor::DecayMode mode) {
    motor_mode = mode;
    update_pwm();
  }

  void Motor::stop() {
    motor_speed = 0.0f;
    update_pwm();
  }

  bool Motor::calculate_pwm_period(float freq, uint16_t& period_out, uint8_t& divider_out) {
    bool success = false;
    if(freq > 0.0f) {
      uint32_t period = (uint32_t)(clock_get_hz(clk_sys) / freq);
      uint8_t divider = 1;

      while((period >= MAX_PWM_PERIOD) && (divider < MAX_PWM_DIVIDER)) {
        period >>= 1;
        divider <<= 1;
      }

      period_out = (uint16_t)std::min(period, MAX_PWM_PERIOD);
      divider_out = divider;

      success = true;
    }
    return success;
  }

  void Motor::update_pwm() {
    int32_t signed_duty_cycle = (int32_t)(std::min(std::max(motor_speed, -1.0f), 1.0f) * (float)pwm_period);

    switch(motor_mode) {
    case FAST_DECAY: //aka 'Coasting'
      if(signed_duty_cycle >= 0) {
        pwm_set_gpio_level(pin_plus, signed_duty_cycle);
        pwm_set_gpio_level(pin_minus, 0);
      }
      else {
        pwm_set_gpio_level(pin_plus, 0);
        pwm_set_gpio_level(pin_minus, 0 - signed_duty_cycle);
      }
      break;

    case SLOW_DECAY: //aka 'Braking'
      if(signed_duty_cycle >= 0) {
        pwm_set_gpio_level(pin_plus, pwm_period);
        pwm_set_gpio_level(pin_minus, pwm_period - signed_duty_cycle);
      }
      else {
        pwm_set_gpio_level(pin_plus, pwm_period + signed_duty_cycle);
        pwm_set_gpio_level(pin_minus, pwm_period);
      }
      break;
    }
  }
};