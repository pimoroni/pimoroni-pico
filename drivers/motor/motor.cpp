#include "motor.hpp"
#include "hardware/clocks.h"
#include <math.h>

namespace pimoroni {
  Motor::Motor(uint pin_pos, uint pin_neg, float freq, DecayMode mode)
    : pin_pos(pin_pos), pin_neg(pin_neg), pwm_frequency(freq), motor_decay_mode(mode) {
  }

  Motor::~Motor() {
    gpio_set_function(pin_pos, GPIO_FUNC_NULL);
    gpio_set_function(pin_neg, GPIO_FUNC_NULL);
  }

  bool Motor::init() {
    bool success = false;

    uint16_t period; uint8_t divider;
    if(Motor::calculate_pwm_period(pwm_frequency, period, divider)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      //Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, period - 1);

      //Apply the divider
      pwm_config_set_clkdiv_int(&pwm_cfg, divider);

      pwm_init(pwm_gpio_to_slice_num(pin_pos), &pwm_cfg, true);
      gpio_set_function(pin_pos, GPIO_FUNC_PWM);

      pwm_init(pwm_gpio_to_slice_num(pin_neg), &pwm_cfg, true);
      gpio_set_function(pin_neg, GPIO_FUNC_PWM);
      update_pwm();

      success = true;
    }
    return success;
  }

  float Motor::get_speed() {
    return motor_speed;
  }

  void Motor::set_speed(float speed) {
    motor_speed = std::min(std::max(speed, -1.0f), 1.0f);
    update_pwm();
  }

  float Motor::get_frequency() {
    return pwm_frequency;
  }

  void Motor::set_frequency(float freq) {
    //Calculate a suitable pwm wrap period for this frequency
    uint16_t period; uint8_t divider;
    if(Motor::calculate_pwm_period(freq, period, divider)) {

      //Record if the new period will be larger or smaller.
      //This is used to apply new pwm values either before or after the wrap is applied,
      //to avoid momentary blips in PWM output on SLOW_DECAY
      bool pre_update_pwm = (period > pwm_period);

      pwm_period = period;
      pwm_frequency = freq;

      //Apply the new divider
      pwm_set_clkdiv_int_frac(pwm_gpio_to_slice_num(pin_pos), divider, 0);
      pwm_set_clkdiv_int_frac(pwm_gpio_to_slice_num(pin_neg), divider, 0);

      //If the the period is larger, update the pwm before setting the new wraps
      if(pre_update_pwm)
        update_pwm();

      //Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_set_wrap(pwm_gpio_to_slice_num(pin_pos), pwm_period - 1);
      pwm_set_wrap(pwm_gpio_to_slice_num(pin_neg), pwm_period - 1);

      //If the the period is smaller, update the pwm after setting the new wraps
      if(!pre_update_pwm)
        update_pwm();
    }
  }

  Motor::DecayMode Motor::get_decay_mode() {
    return motor_decay_mode;
  }

  void Motor::set_decay_mode(Motor::DecayMode mode) {
    motor_decay_mode = mode;
    update_pwm();
  }

  void Motor::stop() {
    motor_speed = 0.0f;
    update_pwm();
  }

  void Motor::disable() {
    motor_speed = 0.0f;
    pwm_set_gpio_level(pin_pos, 0);
    pwm_set_gpio_level(pin_neg, 0);
  }

  bool Motor::calculate_pwm_period(float freq, uint16_t& period_out, uint8_t& divider_out) {
    bool success = false;
    if((freq > 0.0f) && (freq <= (clock_get_hz(clk_sys) >> 1))) { //Half clock is to give a period of at least 2
      uint32_t period = (uint32_t)(clock_get_hz(clk_sys) / freq);
      uint8_t divider = 1;

      while((period > MAX_PWM_PERIOD) && (divider < MAX_PWM_DIVIDER)) {
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
    int32_t signed_duty_cycle = (int32_t)(motor_speed * (float)pwm_period);

    switch(motor_decay_mode) {
    case SLOW_DECAY: //aka 'Braking'
      if(signed_duty_cycle >= 0) {
        pwm_set_gpio_level(pin_pos, pwm_period);
        pwm_set_gpio_level(pin_neg, pwm_period - signed_duty_cycle);
      }
      else {
        pwm_set_gpio_level(pin_pos, pwm_period + signed_duty_cycle);
        pwm_set_gpio_level(pin_neg, pwm_period);
      }
      break;

    case FAST_DECAY: //aka 'Coasting'
    default:
      if(signed_duty_cycle >= 0) {
        pwm_set_gpio_level(pin_pos, signed_duty_cycle);
        pwm_set_gpio_level(pin_neg, 0);
      }
      else {
        pwm_set_gpio_level(pin_pos, 0);
        pwm_set_gpio_level(pin_neg, 0 - signed_duty_cycle);
      }
      break;
    }
  }
};