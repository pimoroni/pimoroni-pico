#include <string.h>
#include <math.h>
#include <functional>

#include "hardware/pwm.h"
#include "hardware/watchdog.h"

#include "inky_frame_7.hpp"

namespace pimoroni {
  void gpio_configure(uint gpio, bool dir, bool value = false) {
    gpio_set_function(gpio, GPIO_FUNC_SIO); gpio_set_dir(gpio, dir); gpio_put(gpio, value);
  }

  void gpio_configure_pwm(uint gpio) {
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(gpio), 65535);
    pwm_init(pwm_gpio_to_slice_num(gpio), &cfg, true);
    gpio_set_function(gpio, GPIO_FUNC_PWM);
  }

  void InkyFrame::init() {
    // keep the pico awake by holding vsys_en high
    gpio_set_function(HOLD_VSYS_EN, GPIO_FUNC_SIO);
    gpio_set_dir(HOLD_VSYS_EN, GPIO_OUT);
    gpio_put(HOLD_VSYS_EN, true);

    // setup the shift register
    gpio_configure(SR_CLOCK, GPIO_OUT, true);
    gpio_configure(SR_LATCH, GPIO_OUT, true);
    gpio_configure(SR_OUT, GPIO_IN);

    // determine wake up event
    if(read_shift_register_bit(BUTTON_A)) {_wake_up_event = BUTTON_A_EVENT;}
    if(read_shift_register_bit(BUTTON_B)) {_wake_up_event = BUTTON_B_EVENT;}
    if(read_shift_register_bit(BUTTON_C)) {_wake_up_event = BUTTON_C_EVENT;}
    if(read_shift_register_bit(BUTTON_D)) {_wake_up_event = BUTTON_D_EVENT;}
    if(read_shift_register_bit(BUTTON_E)) {_wake_up_event = BUTTON_E_EVENT;}
    if(read_shift_register_bit(RTC_ALARM)) {_wake_up_event = RTC_ALARM_EVENT;}
    if(read_shift_register_bit(EXTERNAL_TRIGGER)) {_wake_up_event = EXTERNAL_TRIGGER_EVENT;}
    // there are other reasons a wake event can occur: connect power via usb,
    // connect a battery, or press the reset button. these cannot be
    // disambiguated so we don't attempt to report them

    // Disable display update busy wait, we'll handle it ourselves
    inky73.set_blocking(false);

    // initialise the rtc
    rtc.init();

    // setup led pwm
    gpio_configure_pwm(LED_A);
    gpio_configure_pwm(LED_B);
    gpio_configure_pwm(LED_C);
    gpio_configure_pwm(LED_D);
    gpio_configure_pwm(LED_E);
    gpio_configure_pwm(LED_ACTIVITY);
    gpio_configure_pwm(LED_CONNECTION);
  }

  bool InkyFrame::is_busy() {
    // check busy flag on shift register
    bool busy = !read_shift_register_bit(Flags::EINK_BUSY);
    return busy;
  }

  void InkyFrame::update(bool blocking) {
    while(is_busy()) {
      tight_loop_contents();
    }
    inky73.update((PicoGraphics_PenInky7 *)this);
    while(is_busy()) {
      tight_loop_contents();
    }
    inky73.power_off();
  }

  bool InkyFrame::pressed(Button button) {
    return read_shift_register_bit(button);
  }

  // set the LED brightness by generating a gamma corrected target value for
  // the 16-bit pwm channel. brightness values are from 0 to 100.
  void InkyFrame::led(LED led, uint8_t brightness) {
    uint16_t value =
      (uint16_t)(pow((float)(brightness) / 100.0f, 2.8) * 65535.0f + 0.5f);
    pwm_set_gpio_level(led, value);
  }

  uint8_t InkyFrame::read_shift_register() {
    gpio_put(SR_LATCH, false); sleep_us(1);
    gpio_put(SR_LATCH, true); sleep_us(1);

    uint8_t result = 0;
    uint8_t bits = 8;
    while(bits--) {
      result <<= 1;
      result |= gpio_get(SR_OUT) ? 1 : 0;

      gpio_put(SR_CLOCK, false); sleep_us(1);
      gpio_put(SR_CLOCK, true); sleep_us(1);
    }

    return result;
  }

  bool InkyFrame::read_shift_register_bit(uint8_t index) {
    return read_shift_register() & (1U << index);
  }

  void InkyFrame::sleep(int wake_in_minutes) {
    if(wake_in_minutes != -1) {
      // set an alarm to wake inky up in wake_in_minutes - the maximum sleep
      // is 255 minutes or around 4.5 hours which is the longest timer the RTC
      // supports, to sleep any longer we need to specify a date and time to
      // wake up
      rtc.set_timer(wake_in_minutes, PCF85063A::TIMER_TICK_1_OVER_60HZ);
      rtc.enable_timer_interrupt(true, false);
    }

    // release the vsys hold pin so that inky can go to sleep
    gpio_put(HOLD_VSYS_EN, false);
    while(true){};
  }

  void InkyFrame::sleep_until(int second, int minute, int hour, int day) {
    if(second != -1 || minute != -1 || hour != -1 || day != -1) {
      // set an alarm to wake inky up at the specified time and day
      rtc.set_alarm(second, minute, hour, day);
      rtc.enable_alarm_interrupt(true);
    }

    // release the vsys hold pin so that inky can go to sleep
    gpio_put(HOLD_VSYS_EN, false);
  }

  // Display a portion of an image (icon sheet) at dx, dy
  void InkyFrame::icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy) {
    image(data, sheet_width, icon_size * index, 0, icon_size, icon_size, dx, dy);
  }

  // Display an image that fills the screen (286*128)
  void InkyFrame::image(const uint8_t* data) {
    image(data, width, 0, 0, width, height, 0, 0);
  }

  // Display an image smaller than the screen (sw*sh) at dx, dy
  void InkyFrame::image(const uint8_t *data, int w, int h, int x, int y) {
    image(data, w, 0, 0, w, h, x, y);
  }

  void InkyFrame::image(const uint8_t *data, int stride, int sx, int sy, int dw, int dh, int dx, int dy) {
    for(auto y = 0; y < dh; y++) {
      for(auto x = 0; x < dw; x++) {

        uint32_t o = ((y + sy) * (stride / 2)) + ((x + sx) / 2);
        uint8_t  d = ((x + sx) & 0b1) ? data[o] >> 4 : data[o] & 0xf;

        // draw the pixel
        set_pen(d);
        pixel({dx + x, dy + y});
      }
    }
  }

}
