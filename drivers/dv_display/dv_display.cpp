#include "dv_display.hpp"
#include "swd_load.hpp"
#include "pico-stick.h"

#include <cstdlib>
#include <math.h>
#include <string.h>
#ifndef MICROPY_BUILD_TYPE
#define mp_printf(_, ...) printf(__VA_ARGS__);
#else
extern "C" {
#include "py/runtime.h"
}
#endif

namespace pimoroni {

  void DVDisplay::init(uint16_t display_width_, uint16_t display_height_, Mode mode_, uint16_t frame_width_, uint16_t frame_height_) {
    display_width = display_width_;
    display_height = display_height_;
    mode = mode_;

    if (frame_width_ == 0) frame_width = display_width_;
    else frame_width = frame_width_;

    if (frame_height_ == 0) frame_height = display_height_;
    else frame_height = frame_height_;

    bank = 0;
    h_repeat = 1;
    v_repeat = 1;

    uint8_t res_mode = 0xFF;
    uint16_t full_width = display_width;
    uint16_t full_height = display_height;

    if (display_width < 640 || (display_width == 640 && (display_height == 360 || display_height == 720))) {
      h_repeat = 2;
      full_width *= 2;
    }
    if (display_height < 400) {
      v_repeat = 2;
      full_height *= 2;
    }

    if (full_width == 640) {
      res_mode = 0;
    }
    else if (full_width == 720) {
      if (full_height == 480) res_mode = 1;
      else if (full_height == 400) res_mode = 2;
      else if (full_height == 576) res_mode = 3;
    }
    else if (full_width == 800) {
      if (full_height == 600) res_mode = 0x10;
      else if (full_height == 480) res_mode = 0x11;
      else if (full_height == 450) res_mode = 0x12;
    }
    else if (full_width == 960) {
      if (full_height == 540) res_mode = 0x14;
    }
    else if (full_width == 1280) {
      if (full_height == 720) res_mode = 0x15;
    }

    if (res_mode == 0xFF) {
      mp_printf(&mp_plat_print, "Resolution %dx%d is not supported.  Will use 720x480.\n", display_width, display_height);
    }

    gpio_init(RAM_SEL);
    gpio_put(RAM_SEL, 0);
    gpio_set_dir(RAM_SEL, GPIO_OUT);

    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);

    sleep_ms(200);
    swd_load_program(section_addresses, section_data, section_data_len, sizeof(section_addresses) / sizeof(section_addresses[0]), 0x20000001, 0x15004000, true);

    ram.init();
    bank = 0;
    write_header();
    sleep_us(100);

    gpio_put(RAM_SEL, 1);
    ram.init();
    bank = 1;
    write_header();
    sleep_us(100);

    bank = 0;
    gpio_put(RAM_SEL, 0);
    sleep_ms(100);

    mp_printf(&mp_plat_print, "Start I2C\n");

    if (res_mode != 0xFF) {
      i2c->reg_write_uint8(I2C_ADDR, I2C_REG_SET_RES, res_mode);
    }

    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_START, 1);
    mp_printf(&mp_plat_print, "Started\n");
  }
  
  void DVDisplay::flip() {
    if (mode == MODE_PALETTE) {
      write_palette();
      if (pixel_buffer_location.y != -1) {
        ram.write(point_to_address_palette(pixel_buffer_location), pixel_buffer, pixel_buffer_x);
        pixel_buffer_location.y = -1;
      }
    }
    else if (pixel_buffer_location.y != -1) {
      ram.write(point_to_address16(pixel_buffer_location), pixel_buffer, pixel_buffer_x << 1);
      pixel_buffer_location.y = -1;
    }
    bank ^= 1;
    ram.wait_for_finish_blocking();
    while (gpio_get(VSYNC) == 0);
    gpio_put(RAM_SEL, bank);
    if (rewrite_header) {
      set_scroll_idx_for_lines(-1, 0, display_height);
      rewrite_header = false;
    }
  }

  void DVDisplay::reset() {
    swd_reset();
  }

  void DVDisplay::set_display_offset(const Point& p, int idx) {
    int32_t offset = (int32_t)point_to_address(p) - (int32_t)point_to_address({0,0});
    i2c->write_bytes(I2C_ADDR, I2C_REG_SCROLL_BASE + 4*(idx-1), (uint8_t*)&offset, 4);
  }

  void DVDisplay::set_scroll_idx_for_lines(int idx, int miny, int maxy) {
    constexpr int buf_size = 32;
    uint32_t buf[buf_size];
    uint addr = 4 * (7 + miny);
    uint line_type = (uint)mode << 28;
    if (idx >= 0) line_type |= (uint)idx << 30;
    for (int i = miny; i < maxy; i += buf_size) {
      int maxj = std::min(buf_size, maxy - i);
      if (idx >= 0) {
        for (int j = 0; j < maxj; ++j) {
          buf[j] = line_type + ((uint32_t)h_repeat << 24) + ((i + j) * frame_width * 6) + base_address;
        }
      }
      else {
        ram.read_blocking(addr, buf, maxj);
        for (int j = 0; j < maxj; ++j) {
          buf[j] &= 0xC0000000;
          buf[j] |= line_type + ((uint32_t)h_repeat << 24) + ((i + j) * frame_width * 6) + base_address;
        }
      }
      ram.write(addr, buf, maxj * 4);
      ram.wait_for_finish_blocking();
      addr += 4 * maxj;
    }
  }

  uint8_t DVDisplay::get_gpio() {
    return i2c->reg_read_uint8(I2C_ADDR, I2C_REG_GPIO);
  }

  uint8_t DVDisplay::get_gpio_hi() {
    return i2c->reg_read_uint8(I2C_ADDR, I2C_REG_GPIO_HI);
  }

  void DVDisplay::i2c_modify_bit(uint8_t reg, uint bit, bool enable) {
    uint8_t val = i2c->reg_read_uint8(I2C_ADDR, reg);
    if (enable) val |= 1u << bit;
    else val &= ~(1u << bit);
    i2c->reg_write_uint8(I2C_ADDR, reg, val);
  }

  void DVDisplay::set_gpio_hi_dir(uint pin, bool output) {
    i2c_modify_bit(I2C_REG_GPIO_HI_OE, pin, output);
  }

  void DVDisplay::set_gpio_hi_dir_all(uint8_t val) {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_GPIO_HI_OE, val);
  }

  void DVDisplay::set_gpio_hi(uint pin, bool on) {
    i2c_modify_bit(I2C_REG_GPIO_HI_OUT, pin, on);
  }

  void DVDisplay::set_gpio_hi_all(uint8_t val) {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_GPIO_HI_OUT, val);
  }

  void DVDisplay::set_gpio_hi_pull_up(uint pin, bool on) {
    i2c_modify_bit(I2C_REG_GPIO_HI_PULL_UP, pin, on);
  }

  void DVDisplay::set_gpio_hi_pull_up_all(uint8_t val) {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_GPIO_HI_PULL_UP, val);
  }

  void DVDisplay::set_gpio_hi_pull_down(uint pin, bool on) {
    i2c_modify_bit(I2C_REG_GPIO_HI_PULL_DOWN, pin, on);
  }

  void DVDisplay::set_gpio_hi_pull_down_all(uint8_t val) {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_GPIO_HI_PULL_DOWN, val);
  }

  void DVDisplay::set_led_level(uint8_t level) {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_LED, level | 0x80);
  }

  void DVDisplay::set_led_heartbeat() {
    i2c->reg_write_uint8(I2C_ADDR, I2C_REG_LED, 2);
  }

  void DVDisplay::get_edid(uint8_t* edid) {
    i2c->read_bytes(I2C_ADDR, I2C_REG_EDID, edid, 128);
  }

  void DVDisplay::write(uint32_t address, size_t len, const uint16_t colour)
  {
    uint32_t val = colour | ((uint32_t)colour << 16);

    ram.write_repeat(address, val, len << 1);
  }

  void DVDisplay::write(uint32_t address, size_t len, const RGB888 colour)
  {
    const int VAL_BUFFER_LEN = 60;
    const int VAL_BUFFER_LEN_IN_PIXELS = (VAL_BUFFER_LEN / 3) * 4;
    uint32_t vals[VAL_BUFFER_LEN];

    for (int i = 0, j = 0; i < VAL_BUFFER_LEN_IN_PIXELS && i < (int)len; i += 4) {
      vals[j++] = colour | (colour << 24);
      vals[j++] = (colour >> 8) | (colour << 16);
      vals[j++] = (colour >> 16) | (colour << 8);
    }

    for (int len_bytes = len * 3; len_bytes > 0; len_bytes -= VAL_BUFFER_LEN_IN_PIXELS * 3) {
      uint len_to_write = std::min(len_bytes, VAL_BUFFER_LEN_IN_PIXELS * 3);
      ram.write(address, vals, len_to_write);
      address += len_to_write;
    }
    ram.wait_for_finish_blocking();
  }

  void DVDisplay::read(uint32_t address, size_t len, uint16_t *data)
  {
    ram.read_blocking(address, (uint32_t*)data, (len + 1) >> 1);
  }

  void DVDisplay::write(uint32_t address, size_t len, const uint8_t colour)
  {
    uint32_t val = colour | ((uint32_t)colour << 16);
    val |= val << 8;

    ram.write_repeat(address, val, len);
  }

  void DVDisplay::read(uint32_t address, size_t len, uint8_t *data)
  {
    ram.read_blocking(address, (uint32_t*)data, len);
  }

  void DVDisplay::write_pixel(const Point &p, uint16_t colour)
  {
    if (pixel_buffer_location.y == p.y && pixel_buffer_location.x + pixel_buffer_x == p.x) {
      if (pixel_buffer_x & 1) pixel_buffer[pixel_buffer_x >> 1] |= (uint32_t)colour << 16;
      else pixel_buffer[pixel_buffer_x >> 1] = colour;
      if (++pixel_buffer_x == PIXEL_BUFFER_LEN_IN_WORDS * 2) {
        ram.write(point_to_address16(pixel_buffer_location), pixel_buffer, PIXEL_BUFFER_LEN_IN_WORDS * 4);
        pixel_buffer_location.y = -1;
      }
      return;
    }
    else if (pixel_buffer_location.y != -1) {
      ram.write(point_to_address16(pixel_buffer_location), pixel_buffer, pixel_buffer_x << 1);
    }
    pixel_buffer_location = p;
    pixel_buffer_x = 1;
    pixel_buffer[0] = colour;
  }

  void DVDisplay::write_pixel_span(const Point &p, uint l, uint16_t colour)
  {
    write(point_to_address16(p), l, colour);
  }

  void DVDisplay::write_pixel(const Point &p, RGB888 colour)
  {
    ram.write(point_to_address24(p), &colour, 3);
  }

  void DVDisplay::write_pixel_span(const Point &p, uint l, RGB888 colour)
  {
    write(point_to_address24(p), l, colour);
  }

  void DVDisplay::write_pixel_span(const Point &p, uint l, uint16_t *data)
  {
    uint32_t offset = 0;
    if (((uintptr_t)data & 0x2) != 0) {
      uint32_t val = *data++;
      ram.write(point_to_address16(p), &val, 2);
      --l;
      offset = 2;
    }
    if (l > 0) {
      ram.write(point_to_address16(p) + offset, (uint32_t*)data, l << 1);
    }
  }

  void DVDisplay::read_pixel_span(const Point &p, uint l, uint16_t *data)
  {
    read(point_to_address16(p), l, data);
  }

  void DVDisplay::set_mode(Mode new_mode)
  {
    mode = new_mode;
    rewrite_header = true;
    set_scroll_idx_for_lines(-1, 0, display_height);
    if (mode == MODE_PALETTE) {
      write_palette();
    }
  }
  
  void DVDisplay::set_palette(RGB888 new_palette[PALETTE_SIZE])
  {
    for (int i = 0; i < PALETTE_SIZE; ++i) {
      set_palette_colour(i, new_palette[i]);
    }
  }

  void DVDisplay::set_palette_colour(uint8_t entry, RGB888 colour)
  {
    palette[entry * 3] = (colour >> 16) & 0xFF;
    palette[entry * 3 + 1] = (colour >> 8) & 0xFF;
    palette[entry * 3 + 2] = colour & 0xFF;
  }
  
  void DVDisplay::write_palette()
  {
    uint addr = (display_height + 7) * 4;
    ram.write(addr, (uint32_t*)palette, PALETTE_SIZE * 3);
  }

  void DVDisplay::write_palette_pixel(const Point &p, uint8_t colour)
  {
    if (pixel_buffer_location.y == p.y && pixel_buffer_location.x + pixel_buffer_x == p.x) {
      if (pixel_buffer_x & 3) pixel_buffer[pixel_buffer_x >> 2] |= (uint32_t)colour << ((pixel_buffer_x & 3) << 3);
      else pixel_buffer[pixel_buffer_x >> 2] = colour;
      if (++pixel_buffer_x == PIXEL_BUFFER_LEN_IN_WORDS * 4) {
        ram.write(point_to_address_palette(pixel_buffer_location), pixel_buffer, PIXEL_BUFFER_LEN_IN_WORDS * 4);
        pixel_buffer_location.y = -1;
      }
      return;
    }
    else if (pixel_buffer_location.y != -1) {
      ram.write(point_to_address_palette(pixel_buffer_location), pixel_buffer, pixel_buffer_x);
    }
    pixel_buffer_location = p;
    pixel_buffer_x = 1;
    pixel_buffer[0] = colour;
  }
  
  void DVDisplay::write_palette_pixel_span(const Point &p, uint l, uint8_t colour)
  {
    write(point_to_address_palette(p), l, colour);
  }
  
  void DVDisplay::write_palette_pixel_span(const Point &p, uint l, uint8_t* data)
  {
    uint32_t offset = 0;
    while (((uintptr_t)data & 0x3) != 0 && l > 0) {
      uint32_t val = *data++;
      ram.write(point_to_address_palette(p), &val, 1);
      --l;
      offset++;
      ram.wait_for_finish_blocking();
    }
    if (l > 0) {
      ram.write(point_to_address_palette(p) + offset, (uint32_t*)data, l);
    }
  }
  
  void DVDisplay::read_palette_pixel_span(const Point &p, uint l, uint8_t *data)
  {
    read(point_to_address_palette(p), l, data);
  }

  void DVDisplay::write_header_preamble()
  {
    uint32_t buf[8];
    uint32_t full_width = display_width * h_repeat;
    buf[0] = 0x4F434950;
    buf[1] = 0x01000101 + ((uint32_t)v_repeat << 16);
    buf[2] = full_width << 16;
    buf[3] = (uint32_t)display_height << 16;
    buf[4] = 0x00000001;
    buf[5] = 0x00010000 + display_height + ((uint32_t)bank << 24);
    buf[6] = 0x04000001;
    ram.write(0, buf, 7 * 4);
    ram.wait_for_finish_blocking();
  }

  void DVDisplay::write_sprite_table()
  {
    constexpr uint32_t buf_size = 32;
    uint32_t buf[buf_size];

    uint addr = (display_height + 7) * 4 + PALETTE_SIZE * 3;
    uint sprite_type = (uint)mode << 28;
    for (uint32_t i = 0; i < max_num_sprites; i += buf_size) {
      for (uint32_t j = 0; j < buf_size; ++j) {
        buf[j] = sprite_type + (i + j) * sprite_size + sprite_base_address;
      }
      ram.write(addr, buf, buf_size * 4);
      ram.wait_for_finish_blocking();
      addr += buf_size * 4;
    }
  }

  void DVDisplay::write_header()
  {
    write_header_preamble();
    set_scroll_idx_for_lines(1, 0, display_height);
    write_sprite_table();
  }

  void DVDisplay::define_sprite_internal(uint16_t sprite_data_idx, uint16_t width, uint16_t height, uint32_t* data)
  {
    uint32_t buf[33];
    uint16_t* buf_ptr = (uint16_t*)buf;
    uint addr = sprite_base_address + sprite_data_idx * sprite_size;
    
    *buf_ptr++ = (height << 8) + width;

    for (uint16_t i = 0; i < height; ++i)
    {
      *buf_ptr++ = width << 8;
    }
    uint len = (uint8_t*)buf_ptr - (uint8_t*)buf;
    ram.write(addr, buf, len);

    addr += len;
    if (len & 2) addr += 2;

    ram.write(addr, (uint32_t*)data, width * height * 2);
  }

  void DVDisplay::define_sprite(uint16_t sprite_data_idx, uint16_t width, uint16_t height, uint16_t* data)
  {
    define_sprite_internal(sprite_data_idx, width, height, (uint32_t*)data);
    ram.wait_for_finish_blocking();
  }

  void DVDisplay::define_palette_sprite(uint16_t sprite_data_idx, uint16_t width, uint16_t height, uint8_t* data)
  {
    define_sprite_internal(sprite_data_idx, width, height, (uint32_t*)data);
    ram.wait_for_finish_blocking();
  }

  void DVDisplay::load_pvs_sprite(uint16_t sprite_data_idx, uint32_t* data, uint32_t len_in_bytes)
  {
    uint addr = sprite_base_address + sprite_data_idx * sprite_size;
    ram.write(addr, data, len_in_bytes);
    ram.wait_for_finish_blocking();
  }

  void DVDisplay::set_sprite(int sprite_num, uint16_t sprite_data_idx, const Point &p, SpriteBlendMode blend_mode)
  {
    uint8_t buf[7];
    buf[0] = (uint8_t)blend_mode;
    buf[1] = sprite_data_idx & 0xff;
    buf[2] = sprite_data_idx >> 8;
    buf[3] = p.x & 0xff;
    buf[4] = p.x >> 8;
    buf[5] = p.y & 0xff;
    buf[6] = p.y >> 8;

    i2c->write_bytes(I2C_ADDR, sprite_num, buf, 7);
  }

  void DVDisplay::clear_sprite(int sprite_num)
  {
    uint8_t buf[3] = {1, 0xFF, 0xFF};
    i2c->write_bytes(I2C_ADDR, sprite_num, buf, 3);
  }

  uint32_t DVDisplay::point_to_address(const Point& p) const {
    switch(mode) {
      default:
      case MODE_RGB555: return point_to_address16(p);
      case MODE_PALETTE: return point_to_address_palette(p);
      case MODE_RGB888: return point_to_address24(p);
    }
  }

  int DVDisplay::pixel_size() const {
    switch(mode) {
      default:
      case MODE_RGB555: return 2;
      case MODE_PALETTE: return 1;
      case MODE_RGB888: return 3;
    }
  }
}
