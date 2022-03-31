#include "bitmap_fonts.hpp"
#include "common/unicode_sorta.hpp"

namespace bitmap {
  int32_t measure_character(const font_t *font, const char c, const uint8_t scale) {
    if(c < 32 || c > 127 + 64) { // + 64 char remappings defined in unicode_sorta.hpp
      return 0;
    }

    uint8_t char_index = c;

    if(char_index > 127) {
      char_index = unicode_sorta::char_base[c - 128];
    }

    char_index -= 32;

    return font->widths[char_index] * scale;
  }

  int32_t measure_text(const font_t *font, const std::string &t, const uint8_t scale) {
    int32_t text_width = 0;
    for(auto c : t) {
      text_width += measure_character(font, c, scale);
    }
    return text_width;
  }

  void character(const font_t *font, rect_func rectangle, const char c, const int32_t x, const int32_t y, const uint8_t scale) {
    if(c < 32 || c > 127 + 64) { // + 64 char remappings defined in unicode_sorta.hpp
      return;
    }

    uint8_t char_index = c;
    unicode_sorta::accents char_accent = unicode_sorta::ACCENT_NONE;

    if(char_index > 127) {
      char_index = unicode_sorta::char_base[c - 128];
      char_accent = unicode_sorta::char_accent[c - 128];
    }

    bool upper = char_index < 97; // Only valid for A-Z and a-z
    char_index -= 32;

    const uint8_t *d = &font->data[char_index * font->max_width];
    const uint8_t *a = &font->data[101 * font->max_width + char_accent * font->max_width];

    // Vertical offset of our char within the 32 pixel column canvas
    // At 16 pixels this gives us 8 pixels above and below the char for accents and spacing.
    uint8_t offset = (32 - font->height) / 2;

    for(uint8_t cx = 0; cx < font->widths[char_index]; cx++) {
      // Our maximum bitmap font height will be 16 pixels
      // give ourselves a 32 pixel high canvas in which to plot the char and accent.
      uint32_t data = *d << offset;
      if(char_accent != unicode_sorta::ACCENT_NONE) {
        uint32_t accent = *a;
        accent <<= offset; // Shift the char to the middle of the canvas
        if(char_accent == unicode_sorta::ACCENT_CEDILLA) {
          // Special case handling for the Cedilla- that little upside-down question mark that goes beneath characters
          accent <<= font->accent_offset_below;
        } else {
          accent >>= upper ? font->accent_offset_upper : font->accent_offset_lower; // Shift the accent above the char
        }
        data |= accent; // Merge the accent data into the canvas
      }
      // Offset our y position to account for our column canvas being 32 pixels
      int y_offset = y - (offset * scale);
      // Dra the 32 pixel high column
      for(uint8_t cy = 0; cy < 32; cy++) {
        if((1U << cy) & data) {
          rectangle(x + (cx * scale), y_offset + (cy * scale), scale, scale);
        }
      }

      d++;
      a++;
    }
  }

  void text(const font_t *font, rect_func rectangle, const std::string &t, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale) {
    uint32_t co = 0, lo = 0; // character and line (if wrapping) offset

    size_t i = 0;
    while(i < t.length()) {
      // find length of current word
      size_t next_space = t.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = t.length();
      }

      uint16_t word_width = 0;
      for(size_t j = i; j < next_space; j++) {
        word_width += measure_character(font, t[j], scale);
      }

      // if this word would exceed the wrap limit then
      // move to the next line
      if(co != 0 && co + word_width > (uint32_t)wrap) {
        co = 0;
        lo += (font->height + 1) * scale;
      }

      // draw word
      for(size_t j = i; j < next_space; j++) {
        character(font, rectangle, t[j], x + co, y + lo, scale);
        co += measure_character(font, t[j], scale);
      }

      // move character offset to end of word and add a space
      co += font->widths[0] * scale;
      i = next_space + 1;
    }
  }
}