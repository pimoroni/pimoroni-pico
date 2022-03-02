#include "motor.hpp"
#include "pwm.hpp"

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

    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      //Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, period - 1);

      //Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f);

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
    motor_speed = MIN(MAX(speed, -1.0f), 1.0f);
    update_pwm();
  }

  float Motor::get_frequency() {
    return pwm_frequency;
  }

  bool Motor::set_frequency(float freq) {
    bool success = false;

    //Calculate a suitable pwm wrap period for this frequency
    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(freq, period, div16)) {

      //Record if the new period will be larger or smaller.
      //This is used to apply new pwm values either before or after the wrap is applied,
      //to avoid momentary blips in PWM output on SLOW_DECAY
      bool pre_update_pwm = (period > pwm_period);

      pwm_period = period;
      pwm_frequency = freq;

      uint pos_num = pwm_gpio_to_slice_num(pin_pos);
      uint neg_num = pwm_gpio_to_slice_num(pin_neg);

      //Apply the new divider
      uint8_t div = div16 >> 4;
      uint8_t mod = div16 % 16;
      pwm_set_clkdiv_int_frac(pos_num, div, mod);
      if(neg_num != pos_num) {
        pwm_set_clkdiv_int_frac(neg_num, div, mod);
      }

      //If the the period is larger, update the pwm before setting the new wraps
      if(pre_update_pwm)
        update_pwm();

      //Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_set_wrap(pos_num, pwm_period - 1);
      if(neg_num != pos_num) {
        pwm_set_wrap(neg_num, pwm_period - 1);
      }

      //If the the period is smaller, update the pwm after setting the new wraps
      if(!pre_update_pwm)
        update_pwm();

      success = true;
    }
    return success;
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