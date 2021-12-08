#include <math.h>
#include <string.h>

#include "hardware/gpio.h"  // Workaround SDK bug - https://github.com/raspberrypi/pico-sdk/issues/3
#include "hardware/pwm.h"

#include "pico_display_2.hpp"

const uint8_t LED_R = 6;
const uint8_t LED_G = 7;
const uint8_t LED_B = 8;

namespace pimoroni {

  PicoDisplay2::PicoDisplay2(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, BG_SPI_FRONT)  {
      __fb = buf;
  }

  PicoDisplay2::PicoDisplay2(uint16_t *buf, int width, int height)
    : PicoGraphics(width, height, buf), screen(width, height, buf, BG_SPI_FRONT)  {
      __fb = buf;
  }

  void PicoDisplay2::init() {
    // setup the rgb led for pwm control
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_output_polarity(&cfg, true, true);

    // red
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_R), 65535);
    pwm_init(pwm_gpio_to_slice_num(LED_R), &cfg, true);
    gpio_set_function(LED_R, GPIO_FUNC_PWM);

    // green
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_G), 65535);
    pwm_init(pwm_gpio_to_slice_num(LED_G), &cfg, true);
    gpio_set_function(LED_G, GPIO_FUNC_PWM);

    // blue
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_B), 65535);
    pwm_init(pwm_gpio_to_slice_num(LED_B), &cfg, true);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);

    // setup button inputs
    gpio_set_function(A, GPIO_FUNC_SIO); gpio_set_dir(A, GPIO_IN); gpio_pull_up(A);
    gpio_set_function(B, GPIO_FUNC_SIO); gpio_set_dir(B, GPIO_IN); gpio_pull_up(B);
    gpio_set_function(X, GPIO_FUNC_SIO); gpio_set_dir(X, GPIO_IN); gpio_pull_up(X);
    gpio_set_function(Y, GPIO_FUNC_SIO); gpio_set_dir(Y, GPIO_IN); gpio_pull_up(Y);

    // initialise the screen
    screen.init(true, false);
  }

  void PicoDisplay2::update() {
    screen.update();
  }

  void PicoDisplay2::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

  void PicoDisplay2::set_led(uint8_t r, uint8_t g, uint8_t b) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    static const float gamma = 2.8;

    uint16_t value;

    // red
    value = (uint16_t)(pow((float)(r) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(LED_R, value);

    // green
    value = (uint16_t)(pow((float)(g) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(LED_G, value);

    // blue
    value = (uint16_t)(pow((float)(b) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(LED_B, value);
  }

  bool PicoDisplay2::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

  void PicoDisplay2::flip() {
    screen.flip();
  }
}
