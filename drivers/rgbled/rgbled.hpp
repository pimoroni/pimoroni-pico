#pragma once

#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class RGBLED {
  public:
    RGBLED(uint pin_r, uint pin_g, uint pin_b, Polarity polarity=Polarity::ACTIVE_LOW, uint8_t brightness=255) :
    pin_r(pin_r), pin_g(pin_g), pin_b(pin_b), polarity(polarity), led_brightness(brightness) {
      pwm_cfg = pwm_get_default_config();
      pwm_config_set_wrap(&pwm_cfg, UINT16_MAX);

      pwm_init(pwm_gpio_to_slice_num(pin_r), &pwm_cfg, true);
      gpio_set_function(pin_r, GPIO_FUNC_PWM);

      pwm_init(pwm_gpio_to_slice_num(pin_g), &pwm_cfg, true);
      gpio_set_function(pin_g, GPIO_FUNC_PWM);

      pwm_init(pwm_gpio_to_slice_num(pin_b), &pwm_cfg, true);
      gpio_set_function(pin_b, GPIO_FUNC_PWM);
    };
    ~RGBLED() {
      gpio_set_function(pin_r, GPIO_FUNC_NULL);
      gpio_set_function(pin_g, GPIO_FUNC_NULL);
      gpio_set_function(pin_b, GPIO_FUNC_NULL);
    }
    void set_rgb(uint8_t r, uint8_t g, uint8_t b);
    void set_hsv(float h, float s, float v);
    void set_brightness(uint8_t brightness);
  private:
    uint pin_r;
    uint pin_g;
    uint pin_b;
    Polarity polarity;
    pwm_config pwm_cfg;

    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;
    uint8_t led_brightness = 255;

    void update_pwm();
  };

}