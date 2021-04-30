#include "ltp305.hpp"
#include <algorithm>
#include "dotfont.hpp"

namespace pimoroni {

  bool LTP305::init() {
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    set_brightness(DEFAULT_BRIGHTNESS);
    clear();

    return true;
  }

  i2c_inst_t* LTP305::get_i2c() const {
    return i2c;
  }

  int LTP305::get_sda() const {
    return sda;
  }

  int LTP305::get_scl() const {
    return scl;
  }

  void LTP305::set_brightness(uint8_t brightness, bool update) {

    brightness = std::min(MAX_BRIGHTNESS, brightness);
    if(update)
      i2c_reg_write_uint8(CMD_BRIGHTNESS, brightness);
  }

  void LTP305::set_decimal(bool left, bool right) {
    if(left)
      buf_matrix_left[7 + BUFFER_CMD] |= 0b01000000;
    else
      buf_matrix_left[7 + BUFFER_CMD] &= 0b10111111;

    if(right)
      buf_matrix_right[6 + BUFFER_CMD] |= 0b10000000;
    else
      buf_matrix_right[6 + BUFFER_CMD] &= 0b01111111;
  }

  void LTP305::set_pixel(uint8_t x, uint8_t y, bool c) {
    // Left Matrix
    if(x < 5) {
      if(c)
        buf_matrix_left[x + BUFFER_CMD] |= (0b1 << y);
      else
        buf_matrix_left[x + BUFFER_CMD] &= ~(0b1 << y);
    }
    // Right Matrix
    else {
      x -= 5;
      if(c)
        buf_matrix_right[y + BUFFER_CMD] |= (0b1 << x);
      else
        buf_matrix_right[y + BUFFER_CMD] &= ~(0b1 << x);
    }
  }

  void LTP305::set_character(uint8_t x, uint16_t ch) {
    std::map<uint16_t, DotChar>::const_iterator it = dotfont.find(ch);

    if(it != dotfont.end()) {
      DotChar dchar = it->second;
      for(uint8_t cx = 0; cx < DOT_CHAR_WIDTH; cx++) {
        for(uint8_t cy = 0; cy < HEIGHT; cy++) {
          uint8_t c = dchar.data[cx] & (0b1 << cy);
          set_pixel(x + cx, cy, c);
        }
      }
    }
  }

  void LTP305::set_image(const uint8_t *image, uint16_t image_width, uint16_t image_height,
                         uint16_t offset_x, uint16_t offset_y, bool wrap, bool bg, uint8_t on_level, uint8_t padding) {

    for(uint8_t y = 0; y < HEIGHT; y++) {
      for(uint8_t x = 0; x < WIDTH; x++) {
        bool p = bg;
        uint16_t i_x = x + offset_x;
        uint16_t i_y = y + offset_y;

        if(x >= 5)
          i_x += padding;

        if(wrap) {
          while(i_x >= image_width) {
            i_x -= image_width;
          }
          while(i_y >= image_height) {
            i_y -= image_height;
          }
        }

        if((i_x < image_width) && (i_y < image_height)) {
          p = (image[(i_y * image_width) + i_x] >= on_level);
        }

        set_pixel(x, y, p);
      }
    }
  }

  void LTP305::clear() {
    for(uint8_t i = 0; i < BUFFER_LENGTH; i++) {
      buf_matrix_left[i + 1] = 0x00;
      buf_matrix_right[i + 1] = 0x00;
    }
  }

  void LTP305::show() {
    i2c_write_blocking(i2c, address, buf_matrix_left, BUFFER_CMD + BUFFER_LENGTH, false);
    i2c_write_blocking(i2c, address, buf_matrix_right, BUFFER_CMD + BUFFER_LENGTH, false);
    
    i2c_reg_write_uint8(CMD_MODE, MODE);
    i2c_reg_write_uint8(CMD_OPTIONS, OPTS);
    i2c_reg_write_uint8(CMD_BRIGHTNESS, brightness);
    i2c_reg_write_uint8(CMD_UPDATE, 0x01);
  }

  void LTP305::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
  }
}