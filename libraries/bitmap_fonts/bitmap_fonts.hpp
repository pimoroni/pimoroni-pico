#pragma once

#include <functional>
#include <string>
#include <cstdint>

namespace bitmap {
  struct font_t {
    const uint8_t height;
    const uint8_t max_width;
    const uint8_t accent_offset_upper; // Number of pixels to shift accents UP above uppercase letters
    const uint8_t accent_offset_lower; // Number of pixels to shift accents UP above lowercase letters
    const uint8_t accent_offset_below; // Number of pixels to shift accents DOWN below lowercase letters (height of the cedilla accent)
    const uint8_t widths[96];
    const uint8_t data[];
  };

  typedef std::function<void(int32_t x, int32_t y, int32_t w, int32_t h)> rect_func;

  int32_t measure_character(const font_t *font, const char c, const uint8_t scale);
  int32_t measure_text(const font_t *font, const std::string &t, const uint8_t scale);

  void character(const font_t *font, rect_func rectangle, const char c, const int32_t x, const int32_t y, const uint8_t scale = 2);
  void text(const font_t *font, rect_func rectangle, const std::string &t, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale = 2);  
}