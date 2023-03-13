#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pico_scroll.hpp"
#include "pico_scroll_font.hpp"

enum pin {
  SDA   =  4,
  SCL   =  5,
  A     = 12,
  B     = 13,
  X     = 14,
  Y     = 15,
};

enum reg {
  MODE_REGISTER       = 0x00,
  FRAME_REGISTER      = 0x01,
  AUTOPLAY1_REGISTER  = 0x02,
  AUTOPLAY2_REGISTER  = 0x03,
  BLINK_REGISTER      = 0x05,
  AUDIOSYNC_REGISTER  = 0x06,
  BREATH1_REGISTER    = 0x08,
  BREATH2_REGISTER    = 0x09,
  SHUTDOWN_REGISTER   = 0x0a,
  GAIN_REGISTER       = 0x0b,
  ADC_REGISTER        = 0x0c,
  ENABLE_OFFSET       = 0x00,
  BANK_ADDRESS        = 0xfd,
  COLOR_OFFSET        = 0x24
};

namespace pimoroni {

  PicoScroll::~PicoScroll() {
    clear();
    update();
  }

  void PicoScroll::init() {
    // setup i2c interface
    i2c_init(i2c0, 400000);

    gpio_set_function(pin::SDA, GPIO_FUNC_I2C); gpio_pull_up(pin::SDA);
    gpio_set_function(pin::SCL, GPIO_FUNC_I2C); gpio_pull_up(pin::SCL);

    i2c_write(reg::BANK_ADDRESS, "\x0b", 1);
    i2c_write(reg::MODE_REGISTER, "\x00", 1);
    i2c_write(reg::AUDIOSYNC_REGISTER, "\x00", 1);
    i2c_write(reg::SHUTDOWN_REGISTER, "\x01", 1);
    i2c_write(reg::BANK_ADDRESS, "\x0b", 1);
    i2c_write(reg::FRAME_REGISTER, "\x00", 1);
    i2c_write(reg::BANK_ADDRESS, "\x00", 1);
    i2c_write(reg::ENABLE_OFFSET, "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x00", 18);

    // setup button inputs
    gpio_set_function(pin::A, GPIO_FUNC_SIO); gpio_set_dir(pin::A, GPIO_IN); gpio_pull_up(pin::A);
    gpio_set_function(pin::B, GPIO_FUNC_SIO); gpio_set_dir(pin::B, GPIO_IN); gpio_pull_up(pin::B);
    gpio_set_function(pin::X, GPIO_FUNC_SIO); gpio_set_dir(pin::X, GPIO_IN); gpio_pull_up(pin::X);
    gpio_set_function(pin::Y, GPIO_FUNC_SIO); gpio_set_dir(pin::Y, GPIO_IN); gpio_pull_up(pin::Y);

    // reset the screen
    clear();
    update();
  }

  void PicoScroll::set_pixel(uint8_t x, uint8_t y, uint8_t v) {
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    y = (HEIGHT - 1) - y;
    if(x > 8) {
      x = x - 8;
      y = (HEIGHT - 1) - (y + 8);
    }else{
      x = 8 - x;
    }

    uint8_t o = x * (WIDTH - 1) + y;
    __fb[o] = v;
  }

  bool PicoScroll::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

  void PicoScroll::clear() {
    memset(__fb, 0, BUFFER_SIZE);
  }

  void PicoScroll::set_pixels(const char *pixels) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            set_pixel(x, y, pixels[y * WIDTH + x]);
        }
    }
  }

  void PicoScroll::set_bitmap_1d(const char *bitmap, size_t bitmap_len, int brightness, int offset) {
    for (int x = 0; x < WIDTH; x++) {
      int k = offset + x;
      if ((k >= 0) && (k < (int)bitmap_len)) {
        unsigned char col = bitmap[k];
        for (int y = 0; y < HEIGHT; y++) {
          int val = brightness * ((col >> y) & 1);
          set_pixel(x, y, val);
        }
      }
    }
  }

  void PicoScroll::set_text(const char *text, size_t text_len, int brightness, int offset) {
        int draw_buffer_len = PicoScroll::WIDTH + 7;
        unsigned char draw_buffer[draw_buffer_len];

        // clear the scroll, so only need to write visible bytes
        clear();

        if ((offset < -WIDTH) || (offset > (int)(6 * text_len))) {
            return;
        }

        // compute what can actually be seen, render only that...
        // modify offset and bfr_len accordingly
        if (offset < 0) {
            int space = 1 + (WIDTH + offset) / 6;
            if (space < (int)text_len) {
                text_len = space;
            }
        } else {
            int start = offset / 6;
            offset -= start * 6;
            text_len = text_len - start;
            if (text_len > 4) {
                text_len = 4;
            }
        }

        if (draw_buffer_len > (int)(6 * text_len)) {
            draw_buffer_len = 6 * text_len;
        }

        render_text(text, text_len, draw_buffer, draw_buffer_len);
        set_bitmap_1d((const char *)draw_buffer, draw_buffer_len, brightness, offset);
  }

  void PicoScroll::scroll_text(const char *text, size_t text_len, int brightness, int delay_ms) {
    int draw_buffer_len = 6 * text_len;

    unsigned char *draw_buffer = (unsigned char *)malloc(sizeof(unsigned char) * draw_buffer_len);
    render_text(text, text_len, draw_buffer, draw_buffer_len);

    for (int offset = -WIDTH; offset < draw_buffer_len; offset++) {
        clear();
        set_bitmap_1d((const char *)draw_buffer, draw_buffer_len, brightness, offset);
        update();
        sleep_ms(delay_ms);
    }
    free(draw_buffer);
  }

  void PicoScroll::update() {
    i2c_write(COLOR_OFFSET, (const char *)__fb, BUFFER_SIZE);
  }

  void PicoScroll::i2c_write(uint8_t reg, const char *data, uint8_t len) {
    uint8_t buffer[256];
    buffer[0] = reg;
    memcpy(&buffer[1], data, len);
    i2c_write_blocking(i2c0, DEFAULT_ADDRESS, buffer, len + 1, true);
  }
}