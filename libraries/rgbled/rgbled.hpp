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
    const uint8_t GAMMA[256] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
      2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
      6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
      11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
      19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
      29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
      40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
      55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
      71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
      90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
      111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
      135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
      162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
      191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
      222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255};

    void update_pwm();
  };

}