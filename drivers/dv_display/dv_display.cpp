#include "dv_display.hpp"
#include "swd_load.hpp"
#include "pico-stick-wide.h"

#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {

  void DVDisplay::init() {
    uint8_t mode = 0xFF;
    if (width == 640) {
      mode = 0;
    }
    else if (width == 720) {
      if (height == 480) mode = 1;
      else if (height == 400) mode = 2;
      else if (height == 576) mode = 3;
    }
    else if (width == 800) {
      if (height == 600) mode = 0x10;
      else if (height == 480) mode = 0x11;
      else if (height == 450) mode = 0x12;
    }
    else if (width == 960) {
      if (height == 540) mode = 0x14;
    }
    else if (width == 1280) {
      if (height == 720) mode = 0x15;
    }

    if (mode == 0xFF) {
      printf("Resolution %dx%d is not supported.  Will use 720x480.\n", width, height);
    }

    gpio_init(RAM_SEL);
    gpio_put(RAM_SEL, 0);
    gpio_set_dir(RAM_SEL, GPIO_OUT);

    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);

    sleep_ms(200);
    swd_load_program(section_addresses, section_data, section_data_len, sizeof(section_addresses) / sizeof(section_addresses[0]), 0x20000001, 0x15004000, true);

    ram.init();
    write_header(0);

    gpio_put(RAM_SEL, 1);
    ram.init();
    write_header(1);

    bank = 0;
    gpio_put(RAM_SEL, 0);
    sleep_ms(100);

    printf("Start I2C\n");

    if (mode != 0xFF) {
      i2c.reg_write_uint8(I2C_ADDR, I2C_REG_SET_RES, mode);
    }

    i2c.reg_write_uint8(I2C_ADDR, I2C_REG_START, 1);
    printf("Started\n");
  }
  
  void DVDisplay::flip() {
    if (pixel_buffer_location.y != -1) {
      ram.write(pointToAddress(pixel_buffer_location), pixel_buffer, pixel_buffer_x << 1);
      pixel_buffer_location.y = -1;
    }
    bank ^= 1;
    ram.wait_for_finish_blocking();
    while (gpio_get(VSYNC) == 0);
    gpio_put(RAM_SEL, bank);
  }

  uint8_t DVDisplay::get_driver_gpio() {
    return i2c.reg_read_uint8(I2C_ADDR, I2C_REG_GPIO);
  }

  uint8_t DVDisplay::get_driver_gpio_hi() {
    return i2c.reg_read_uint8(I2C_ADDR, I2C_REG_GPIO_HI);
  }

  void DVDisplay::write(uint32_t address, size_t len, const uint16_t colour)
  {
    uint32_t val = colour | ((uint32_t)colour << 16);

    ram.write_repeat(address, val, len << 1);
  }

  void DVDisplay::read(uint32_t address, size_t len, uint16_t *data)
  {
    ram.read_blocking(address, (uint32_t*)data, (len + 1) >> 1);
  }

  void DVDisplay::write_pixel(const Point &p, uint16_t colour)
  {
    if (pixel_buffer_location.y == p.y && pixel_buffer_location.x + pixel_buffer_x == p.x) {
      if (pixel_buffer_x & 1) pixel_buffer[pixel_buffer_x >> 1] |= (uint32_t)colour << 16;
      else pixel_buffer[pixel_buffer_x >> 1] = colour;
      if (++pixel_buffer_x == PIXEL_BUFFER_LEN_IN_WORDS * 2) {
        ram.write(pointToAddress(pixel_buffer_location), pixel_buffer, PIXEL_BUFFER_LEN_IN_WORDS * 4);
        pixel_buffer_location.y = -1;
      }
      return;
    }
    else if (pixel_buffer_location.y != -1) {
      ram.write(pointToAddress(pixel_buffer_location), pixel_buffer, pixel_buffer_x << 1);
    }
    pixel_buffer_location = p;
    pixel_buffer_x = 1;
    pixel_buffer[0] = colour;
  }

  void DVDisplay::write_pixel_span(const Point &p, uint l, uint16_t colour)
  {
    write(pointToAddress(p), l, colour);
  }

  void DVDisplay::read_pixel_span(const Point &p, uint l, uint16_t *data)
  {
    read(pointToAddress(p), l, data);
  }

  void DVDisplay::write_header(uint bank)
  {
    uint32_t buf[8];
    buf[0] = 0x4F434950;
    buf[1] = 0x01010101;
    buf[2] = (uint32_t)width << 16;
    buf[3] = (uint32_t)height << 16;
    buf[4] = 0x00000001;
    buf[5] = 0x00010000 + height + (bank << 24);
    buf[6] = 0x00000000;
    ram.write(0, buf, 7 * 4);
    ram.wait_for_finish_blocking();

    uint addr = 4 * 7;
    for (int i = 0; i < height; i += 8) {
      for (int j = 0; j < 8; ++j) {
        buf[j] = 0x91000000 + ((i + j) * width * 2) + base_address;
      }
      ram.write(addr, buf, 8 * 4);
      ram.wait_for_finish_blocking();
      addr += 4 * 8;
    }

    sleep_us(100);
  }
}
