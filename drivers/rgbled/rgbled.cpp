#include "rgbled.hpp"

namespace pimoroni {
  void RGBLED::set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    led_r = r;
    led_g = g;
    led_b = b;
    update_pwm();
  }

  void RGBLED::set_brightness(uint8_t brightness) {
    led_brightness = brightness;
    update_pwm();
  }

  void RGBLED::set_hsv(float h, float s, float v) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: led_r = v; led_g = t; led_b = p; break;
      case 1: led_r = q; led_g = v; led_b = p; break;
      case 2: led_r = p; led_g = v; led_b = t; break;
      case 3: led_r = p; led_g = q; led_b = v; break;
      case 4: led_r = t; led_g = p; led_b = v; break;
      case 5: led_r = v; led_g = p; led_b = q; break;
    }

    update_pwm();
  }

  void RGBLED::update_pwm() {
    uint16_t r16 = GAMMA_8BIT[led_r];
    uint16_t g16 = GAMMA_8BIT[led_g];
    uint16_t b16 = GAMMA_8BIT[led_b];
    r16 *= led_brightness;
    g16 *= led_brightness;
    b16 *= led_brightness;
    if(polarity == Polarity::ACTIVE_LOW) {
      r16 = UINT16_MAX - r16;
      g16 = UINT16_MAX - g16;
      b16 = UINT16_MAX - b16;
    }
    pwm_set_gpio_level(pin_r, r16);
    pwm_set_gpio_level(pin_g, g16);
    pwm_set_gpio_level(pin_b, b16);
  }
};