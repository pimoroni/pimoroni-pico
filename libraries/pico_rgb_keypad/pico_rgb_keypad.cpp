#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"

#include "pico_rgb_keypad.hpp"

enum pin {
  SDA       =  4,
  SCL       =  5,
  CS        = 17,
  SCK       = 18,
  MOSI      = 19
};

namespace pimoroni {

  PicoRGBKeypad::~PicoRGBKeypad() {
    clear();
    update();
  }

  void PicoRGBKeypad::init() {
    memset(buffer, 0, sizeof(buffer));
    led_data = buffer + 4;

    set_brightness(DEFAULT_BRIGHTNESS); //Must be called to init each LED frame

    // setup i2c interface
    i2c_init(i2c0, 400000);
    gpio_set_function(pin::SDA, GPIO_FUNC_I2C); gpio_pull_up(pin::SDA);
    gpio_set_function(pin::SCL, GPIO_FUNC_I2C); gpio_pull_up(pin::SCL);

    spi_init(spi0, 4 * 1024 * 1024);
    gpio_set_function(pin::CS, GPIO_FUNC_SIO);
    gpio_set_dir(pin::CS, GPIO_OUT);
    gpio_put(pin::CS, 1);
    gpio_set_function(pin::SCK, GPIO_FUNC_SPI);
    gpio_set_function(pin::MOSI, GPIO_FUNC_SPI);

    update();
  }

  void PicoRGBKeypad::update() {
    gpio_put(pin::CS, 0);
    spi_write_blocking(spi0, buffer, sizeof(buffer));
    gpio_put(pin::CS, 1);
  }

  void PicoRGBKeypad::set_brightness(float brightness) {
    if(brightness < 0.0f || brightness > 1.0f) {
      return;
    }

    for(uint16_t i = 0; i < NUM_PADS; i++)
        led_data[i * 4] = 0b11100000 | (uint8_t)(brightness * (float)0b11111);
  }

  void PicoRGBKeypad::illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      return;
    }

    uint16_t offset = (x + (y * WIDTH)) * 4;
    //led_data[offset + 0] = 0xff;  //Not needed as set at init
    led_data[offset + 1] = b;
    led_data[offset + 2] = g;
    led_data[offset + 3] = r;
  }

  void PicoRGBKeypad::illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
    if(i < 0 || i >= NUM_PADS) {
      return;
    }

    uint16_t offset = i * 4;
    //led_data[offset + 0] = 0xff;  //Not needed as set at init
    led_data[offset + 1] = b;
    led_data[offset + 2] = g;
    led_data[offset + 3] = r;
  }

  void PicoRGBKeypad::clear() {
    for(uint16_t i = 0; i < NUM_PADS; i++)
      illuminate(i, 0, 0, 0);
  }

  uint16_t PicoRGBKeypad::get_button_states() {
    uint8_t i2c_read_buffer[2];
    uint8_t reg = 0;
    i2c_write_blocking(i2c0, KEYPAD_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c0, KEYPAD_ADDRESS, i2c_read_buffer, 2, false);
    return ~((i2c_read_buffer[0]) | (i2c_read_buffer[1] << 8));
  }

}