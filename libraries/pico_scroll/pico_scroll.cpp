#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pico_scroll.hpp"

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