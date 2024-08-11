#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <cstdint>
#include "common/unicode_sorta.hpp"

namespace bitmap {
  const int base_chars = 96;  // 96 printable ASCII chars
  const int extra_chars = 9;  // Extra chars we've rempped that aren't just an ASCII char plus an accent

  struct font_t {
    const uint8_t height;
    const uint8_t max_width;
    const uint8_t widths[base_chars + extra_chars];
    const uint8_t data[];
  };

  typedef std::function<void(int32_t x, int32_t y, int32_t w, int32_t h)> rect_func;

  int32_t measure_character(const font_t *font, const char c, const uint8_t scale, unicode_sorta::codepage_t codepage = unicode_sorta::PAGE_195, bool fixed_width = false);
  int32_t measure_text(const font_t *font, const std::string_view &t, const uint8_t scale = 2, const uint8_t letter_spacing = 1, bool fixed_width = false);

  void character(const font_t *font, rect_func rectangle, const char c, const int32_t x, const int32_t y, const uint8_t scale = 2, int32_t rotation = 0, unicode_sorta::codepage_t codepage = unicode_sorta::PAGE_195);
  void text(const font_t *font, rect_func rectangle, const std::string_view &t, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale = 2, const uint8_t letter_spacing = 1, bool fixed_width = false, int32_t rotation = 0);
}