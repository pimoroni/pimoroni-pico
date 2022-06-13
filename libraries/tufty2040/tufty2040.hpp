#pragma once

#include <string>
#include <math.h>

#include "hardware/gpio.h"  // Workaround SDK bug - https://github.com/raspberrypi/pico-sdk/issues/3
#include "hardware/pwm.h"

namespace pimoroni {

  class Tufty2040 {
  public:
    static const int WIDTH = 320;
    static const int HEIGHT = 240;
    static const uint8_t A = 7;
    static const uint8_t B = 8;
    static const uint8_t C = 9;
    static const uint8_t UP = 22;
    static const uint8_t DOWN = 6;
    static const uint8_t LED = 25;
    static const uint8_t BACKLIGHT = 2;


    static const uint8_t LCD_CS = 10;
    static const uint8_t LCD_DC = 11;
    static const uint8_t LCD_WR = 12;
    static const uint8_t LCD_RD = 13;
    static const uint8_t LCD_D0 = 14;

  public:
    Tufty2040() {
      
      gpio_set_function(LCD_D0 + 0, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 0, true);
      gpio_set_function(LCD_D0 + 1, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 1, true);
      gpio_set_function(LCD_D0 + 2, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 2, true);
      gpio_set_function(LCD_D0 + 3, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 3, true);
      gpio_set_function(LCD_D0 + 4, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 4, true);
      gpio_set_function(LCD_D0 + 5, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 5, true);
      gpio_set_function(LCD_D0 + 6, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 6, true);
      gpio_set_function(LCD_D0 + 7, GPIO_FUNC_SIO); gpio_set_dir(LCD_D0 + 7, true);

      // led control pin
      pwm_config cfg = pwm_get_default_config();
      pwm_set_wrap(pwm_gpio_to_slice_num(LED), 65535);
      pwm_init(pwm_gpio_to_slice_num(LED), &cfg, true);
      gpio_set_function(LED, GPIO_FUNC_PWM);
      led(0);
    }

    void led(uint8_t brightness) {
      // set the led brightness from 1 to 256 with gamma correction
      float gamma = 2.8;
      uint16_t v = (uint16_t)(pow((float)(brightness) / 256.0f, gamma) * 65535.0f + 0.5f);
      pwm_set_gpio_level(LED, v);
    }

  };

}
