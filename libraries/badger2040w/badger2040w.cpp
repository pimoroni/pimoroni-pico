#include <string.h>
#include <math.h>

#include "hardware/pwm.h"
#include "hardware/watchdog.h"

#include "badger2040w.hpp"

namespace pimoroni {

  void Badger2040W::init() {

    gpio_set_function(ENABLE_3V3, GPIO_FUNC_SIO);
    gpio_set_dir(ENABLE_3V3, GPIO_OUT);
    gpio_put(ENABLE_3V3, 1);

    gpio_set_function(A, GPIO_FUNC_SIO);
    gpio_set_dir(A, GPIO_IN);
    gpio_set_pulls(A, false, true);

    gpio_set_function(B, GPIO_FUNC_SIO);
    gpio_set_dir(B, GPIO_IN);
    gpio_set_pulls(B, false, true);

    gpio_set_function(C, GPIO_FUNC_SIO);
    gpio_set_dir(C, GPIO_IN);
    gpio_set_pulls(C, false, true);

    gpio_set_function(D, GPIO_FUNC_SIO);
    gpio_set_dir(D, GPIO_IN);
    gpio_set_pulls(D, false, true);

    gpio_set_function(E, GPIO_FUNC_SIO);
    gpio_set_dir(E, GPIO_IN);
    gpio_set_pulls(E, false, true);

    // read initial button states
    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E);
    _wake_button_states |= gpio_get_all() & mask;

    // led control pin
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(LED), 65535);
    pwm_init(pwm_gpio_to_slice_num(LED), &cfg, true);
    gpio_set_function(LED, GPIO_FUNC_PWM);
    led(0);

    uc8151 = std::make_unique<UC8151>(296, 128, ROTATE_0);
    graphics = std::make_unique<PicoGraphics_Pen1BitY>(296, 128, nullptr);
  }

  void Badger2040W::halt() {
    gpio_put(ENABLE_3V3, 0);

    // If running on USB we will not actually power down, so emulate the behaviour
    // of battery powered badge by listening for a button press and then resetting
    // Note: Don't use wait_for_press as that waits for the button to be release and
    //       we want the reboot to complete before the button is released.
    update_button_states();
    while(_button_states == 0) {
      update_button_states();
    }
    watchdog_reboot(0, 0, 0);
  }

  void Badger2040W::update_button_states() {
    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E);
    _button_states = gpio_get_all() & mask;
  }

  uint32_t Badger2040W::button_states() {
    return _button_states;
  }

  // // Display a portion of an image (icon sheet) at dx, dy
  // void Badger2040W::icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy) {
  //   image(data, sheet_width, icon_size * index, 0, icon_size, icon_size, dx, dy);
  // }

  void Badger2040W::imageRow(const uint8_t *data, Rect rect) {
    for(auto x = 0; x < rect.w; x++) {
        // work out byte offset in source data
        uint32_t o = (x >> 3);

        // extract bitmask for this pixel
        uint32_t bm = 0b10000000 >> (x & 0b111);

        // set pixel color
        graphics->set_pen(data[o] & bm ? 15 : 0);
        // draw the pixel
        graphics->set_pixel(Point(x + rect.x, rect.y));
    }
  }
  // Display an image smaller than the screen (sw*sh) at dx, dy
  void Badger2040W::image(const uint8_t *data, Rect rect) {
    for(auto y = 0; y < rect.h; y++) {
      const uint8_t *scanline = data + ((y * rect.w) >> 3);
      Badger2040W::imageRow(scanline, Rect(rect.x, y + rect.y, rect.w, 0));
    }
  }

  void Badger2040W::partial_update(Rect region) {
    uc8151->partial_update(graphics.get(), region);
  }

  void Badger2040W::update() {
    uc8151->update(graphics.get());
  }


  void Badger2040W::led(uint8_t brightness) {
    // set the led brightness from 1 to 256 with gamma correction
    float gamma = 2.8;
    uint16_t v = (uint16_t)(pow((float)(brightness) / 256.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(LED, v);
  }

  bool Badger2040W::pressed(uint8_t button) {
    return (_button_states & (1UL << button)) != 0;
  }

  bool Badger2040W::pressed_to_wake(uint8_t button) {
    return (_wake_button_states & (1UL << button)) != 0;
  }

  void Badger2040W::wait_for_press() {
    update_button_states();
    while(_button_states == 0) {
      update_button_states();
      tight_loop_contents();
    }

    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E);
    while(gpio_get_all() & mask) {
      tight_loop_contents();
    }
  }
}
