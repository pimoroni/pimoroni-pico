#include <string.h>
#include <math.h>

#include "hardware/pwm.h"
#include "hardware/watchdog.h"

#include "pimoroni_i2c.hpp"
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

    // PCF85063a handles the initialisation of the RTC GPIO pin
    pcf85063a = std::make_unique<PCF85063A>(new I2C(I2C_BG_SDA, I2C_BG_SCL), (uint)RTC);

    // read initial button states
    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E) | (1UL << RTC);
    _wake_button_states |= gpio_get_all() & mask;

    // led control pin
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(LED), 65535);
    pwm_init(pwm_gpio_to_slice_num(LED), &cfg, true);
    gpio_set_function(LED, GPIO_FUNC_PWM);
    led(0);

    // Initialise display driver and pico graphics library
    uc8151 = std::make_unique<UC8151>(DISPLAY_WIDTH, DISPLAY_HEIGHT, ROTATE_0);
    graphics = std::make_unique<PicoGraphics_Pen1BitY>(DISPLAY_WIDTH, DISPLAY_HEIGHT, nullptr);
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
    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E) | (1UL << RTC);
    _button_states = gpio_get_all() & mask;
  }

  uint32_t Badger2040W::button_states() {
    return _button_states;
  }

  void Badger2040W::imageRow(const uint8_t *data, Rect rect) {
    for(auto x = 0; x < rect.w; x++) {
        // work out byte offset in source data
        uint32_t o = (x >> 3);

        // extract bitmask for this pixel
        uint32_t bm = 0b10000000 >> (x & 0b111);

        // set pixel color
        graphics->set_pen(data[o] & bm ? 0 : 15);
        // draw the pixel
        graphics->set_pixel(Point(x + rect.x, rect.y));
    }
  }

  // Display a portion of an image (icon sheet) at rect 
  void Badger2040W::icon(const uint8_t *data, int index, int sheet_width, Rect rect) {
    for(auto y = 0; y < rect.h; y++) {
      const uint8_t *scanline = data + ((y * sheet_width) >> 3) + ((rect.w * index) >> 3);
      imageRow(scanline, Rect(rect.x, y + rect.y, rect.w, 0));
    }
  }

  // Display an image at rect
  void Badger2040W::image(const uint8_t *data, Rect rect) {
    for(auto y = 0; y < rect.h; y++) {
      const uint8_t *scanline = data + ((y * rect.w) >> 3);
      imageRow(scanline, Rect(rect.x, y + rect.y, rect.w, 0));
    }
  }

  // Display an image that covers the entire screen
  void Badger2040W::image(const uint8_t *data) {
    image(data, Rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT));
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

    uint32_t mask = (1UL << A) | (1UL << B) | (1UL << C) | (1UL << D) | (1UL << E) | (1UL << RTC);
    while(gpio_get_all() & mask) {
      tight_loop_contents();
    }
  }
}
