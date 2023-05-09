#include "dv_display.hpp"

#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {

  void DVDisplay::init() {
    gpio_init(RAM_SEL);
    gpio_put(RAM_SEL, 0);
    gpio_set_dir(RAM_SEL, GPIO_OUT);

    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);

    ram.init();
    write_header(0);

    gpio_put(RAM_SEL, 1);
    ram.init();
    write_header(1);

    bank = 0;
    gpio_put(RAM_SEL, 0);
    i2c.reg_write_uint8(I2C_ADDR, I2C_REG_START, 1);
  }
  
  void DVDisplay::flip() {
    bank ^= 1;
    ram.wait_for_finish_blocking();
    while (gpio_get(VSYNC) == 0);
    gpio_put(RAM_SEL, bank);
  }

  void DVDisplay::write(uint32_t address, size_t len, const uint16_t *data)
  {
    if ((uintptr_t)data & 2) {
      write(address, 1, *data);
      data++;
      len--;
      address += 2;
    }

    if (len > 0) {
      ram.write(address, (uint32_t*)data, len << 1);
    }
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
    write(pointToAddress(p), 1, colour);
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