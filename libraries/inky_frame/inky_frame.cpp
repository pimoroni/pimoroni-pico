#include <string.h>
#include <math.h>
#include <functional>

#include "hardware/pwm.h"
#include "hardware/watchdog.h"

#include "inky_frame.hpp"

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

    // initialise the eink display and provide our custom busy hook as the
    // busy pin is connected to the on board shift register rather than
    // directly to an io pin on the rp2040
    uc8159.init(true);
    uc8159.set_is_busy_callback(is_busy);

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
    uc8159.update(blocking);
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





  // drawing functions

  void InkyFrame::clear() {
    for(int32_t y = 0; y < HEIGHT; y++) {
      for(int32_t x = 0; x < WIDTH; x++) {
        pixel(x, y);
      }
    }
  }

  void InkyFrame::pixel(int32_t x, int32_t y) {
    if(_thickness == 1) {
      uc8159.pixel(x, y, _pen);
    }else{
      int ht = _thickness / 2;
      int rs = ht * ht;
      for(int sy = -ht; sy <= ht; sy++) {
        for(int sx = -ht; sx <= ht; sx++) {
          if(sx * sx + sy * sy <= rs) {
            uc8159.pixel(x + sx, y + sy, _pen);
          }
        }
      }
    }
  }

  void InkyFrame::fast_pixel(int32_t x, int32_t y, uint8_t pen) {
    uc8159.pixel(x, y, pen);
  }

  // Display a portion of an image (icon sheet) at dx, dy
  void InkyFrame::icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy) {
    image(data, sheet_width, icon_size * index, 0, icon_size, icon_size, dx, dy);
  }

  // Display an image that fills the screen (286*128)
  void InkyFrame::image(const uint8_t* data) {
    image(data, WIDTH, 0, 0, WIDTH, HEIGHT, 0, 0);
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
        uc8159.pixel(dx + x, dy + y, d);
      }
    }
  }

  void InkyFrame::rectangle(int32_t x, int32_t y, int32_t w, int32_t h) {
    for(int cy = y; cy < y + h; cy++) {
      for(int cx = x; cx < x + w; cx++) {
        pixel(cx, cy);
      }
    }
  }

  void InkyFrame::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    int32_t	x = x1, y = y1, dx, dy, incx, incy, balance;

    if(x2 >= x1) {dx = x2 - x1; incx = 1;} else {dx = x1 - x2; incx = -1;}
    if(y2 >= y1) {dy = y2 - y1; incy = 1;} else {dy = y1 - y2; incy = -1;}

    if(dx >= dy) {
      dy <<= 1; balance = dy - dx; dx <<= 1;
      while(x != x2) {
        pixel(x, y);
        if (balance >= 0) {y += incy; balance -= dx;}
        balance += dy; x += incx;
      }
    } else {
      dx <<= 1; balance = dx - dy; dy <<= 1;
      while(y != y2) {
        pixel(x, y);
        if(balance >= 0) {x += incx; balance -= dy;}
        balance += dx; y += incy;
      }
    }
  }

  const hershey::font_glyph_t* InkyFrame::glyph_data(unsigned char c) {
    return hershey::glyph_data(_font, c);
  }

  int32_t InkyFrame::glyph(unsigned char c, int32_t x, int32_t y, float s, float a) {
    if (_bitmap_font) {
      bitmap::character(_bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        for(auto px = 0; px < w; px++) {
          for(auto py = 0; py < h; py++) {
            pixel(x + px, y + py);
          }
        }
      }, c, x, y, int(s));
      return 0;
    } else {
      return hershey::glyph(_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
        line(x1, y1, x2, y2);
      }, c, x, y, s, a);
    }
  }

  void InkyFrame::text(std::string message, int32_t x, int32_t y, float s, float a) {
    if (_bitmap_font) {
      bitmap::text(_bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        for(auto px = 0; px < w; px++) {
          for(auto py = 0; py < h; py++) {
            pixel(x + px, y + py);
          }
        }
      }, message, x, y, INT32_MAX, int(s)); // We basically want to disable wrapping, since it wont really work here, but Pico Graphics needs it
    } else {
      hershey::text(_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
        line(x1, y1, x2, y2);
      }, message, x, y, s, s * _text_aspect, a, _text_tracking);
    }
  }

  int32_t InkyFrame::measure_text(std::string message, float s) {
    if (_bitmap_font) return bitmap::measure_text(_bitmap_font, message, s);
    return hershey::measure_text(_font, message, s);
  }

  int32_t InkyFrame::measure_glyph(unsigned char c, float s) {
    if (_bitmap_font) return bitmap::measure_character(_bitmap_font, c, s);
    return hershey::measure_glyph(_font, c, s);
  }

  void InkyFrame::font(std::string name) {
    if (name == "bitmap6") {
      _bitmap_font = &font6;
      _font = nullptr;
    } else if (name == "bitmap8") {
      _bitmap_font = &font8;
      _font = nullptr;
    } else {
      // check that font exists and assign it
      if(hershey::fonts.find(name) != hershey::fonts.end()) {
        _bitmap_font = nullptr;
        _font = hershey::fonts[name];
      }
    }
  }

  void InkyFrame::pen(uint8_t pen) {
    _pen = pen;
  }

  void InkyFrame::text_tracking(float text_tracking) {
    _text_tracking = text_tracking;
  }

  void InkyFrame::text_aspect(float text_aspect) {
    _text_aspect = text_aspect;
  }

  void InkyFrame::thickness(uint8_t thickness) {
    _thickness = thickness;
  }


}
