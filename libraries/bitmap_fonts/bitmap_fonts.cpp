#include "bitmap_fonts.hpp"

namespace bitmap {
  int32_t measure_character(const font_t *font, const char c, const uint8_t scale, unicode_sorta::codepage_t codepage, bool fixed_width) {
    if(c < 32 || c > 127 + 64) { // + 64 char remappings defined in unicode_sorta.hpp
      return 0;
    }

    if(fixed_width) {
      return font->max_width * scale;
    }

    uint8_t char_index = c;

    if(char_index > 127) {
      if(codepage == unicode_sorta::PAGE_195) {
        char_index = unicode_sorta::char_base_195[c - 128];
      } else {
        char_index = unicode_sorta::char_base_194[c - 128 - 32];
      }
    }

    char_index -= 32;

    return font->widths[char_index] * scale;
  }

  int32_t measure_text(const font_t *font, const std::string_view &t, const uint8_t scale, const uint8_t letter_spacing, bool fixed_width) {
    int32_t text_width = 0;
    unicode_sorta::codepage_t codepage = unicode_sorta::PAGE_195;
    for(auto c : t) {
      if(c == unicode_sorta::PAGE_194_START) {
        codepage = unicode_sorta::PAGE_194;
        continue;
      } else if (c == unicode_sorta::PAGE_195_START) {
        continue;
      }
      text_width += measure_character(font, c, scale, codepage, fixed_width);
      text_width += letter_spacing * scale;
      codepage = unicode_sorta::PAGE_195; // Reset back to default
    }
    return text_width;
  }

  void character(const font_t *font, rect_func rectangle, const char c, const int32_t x, const int32_t y, const uint8_t scale, int32_t rotation, unicode_sorta::codepage_t codepage) {
    if(c < 32 || c > 127 + 64) { // + 64 char remappings defined in unicode_sorta.hpp
      return;
    }

    uint8_t char_index = c;
    unicode_sorta::accents char_accent = unicode_sorta::ACCENT_NONE;

    // Remap any chars that fall outside of the 7-bit ASCII range
    // using our unicode fudge lookup table.
    if(char_index > 127) {
      if(codepage == unicode_sorta::PAGE_195) {
        char_index = unicode_sorta::char_base_195[c - 128];
        char_accent = unicode_sorta::char_accent[c - 128];
      } else {
        char_index = unicode_sorta::char_base_194[c - 128 - 32];
        char_accent = unicode_sorta::ACCENT_NONE;
      }
    }

    // We don't map font data for the first 32 non-printable ASCII chars
    char_index -= 32;

    // If our font is taller than 8 pixels it must be two bytes per column
    bool two_bytes_per_column = font->height > 8;

    // Figure out how many bytes we need to skip per char to find our data in the array
    uint8_t bytes_per_char = two_bytes_per_column ? font->max_width * 2 : font->max_width;

    // Get a pointer to the start of the data for this character
    const uint8_t *d = &font->data[char_index * bytes_per_char];

    // Accents can be up to 8 pixels tall on both 8bit and 16bit fonts
    // Each accent's data is font->max_width bytes + 2 offset bytes long
    const uint8_t *a = &font->data[(base_chars + extra_chars) * bytes_per_char + char_accent * (font->max_width + 2)];

    // Effectively shift off the first two bytes of accent data-
    // these are the lower and uppercase accent offsets
    const uint8_t offset_lower = *a++;
    const uint8_t offset_upper = *a++;

    // Pick which offset we should use based on the case of the char
    // This is only valid for A-Z a-z.
    // Note this magic number is relative to the start of printable ASCII chars.
    uint8_t accent_offset = char_index < 65 ? offset_upper : offset_lower;

    // Offset our y position to account for our column canvas being 32 pixels
    // this gives us 8 "pixels" of headroom above the letters for diacritic marks
    int font_offset = (8 * scale);

    // Iterate through each horizontal column of font (and accent) data
    for(uint8_t cx = 0; cx < font->widths[char_index]; cx++) {
      // Our maximum bitmap font height will be 16 pixels
      // give ourselves a 32 pixel high canvas in which to plot the char and accent.
      // We shift the char down 8 pixels to make room for an accent above.
      uint32_t data = *d << 8;

      int32_t o_x = cx * scale;

      // For fonts that are taller than 8 pixels (up to 16) they need two bytes
      if(two_bytes_per_column) {
        d++;
        data <<= 8;      // Move down the first byte
        data |= *d << 8; // Add the second byte
      }

      // If the char has an accent, merge it into the column data at its offset
      if(char_accent != unicode_sorta::ACCENT_NONE) {
        data |= *a << accent_offset;
      }

      // Draw the 32 pixel column
      for(uint8_t cy = 0; cy < 32; cy++) {
        if((1U << cy) & data) {
          int32_t o_y = cy * scale;
          int32_t px = 0;
          int32_t py = 0;
          switch (rotation) {
            case 0:
              px = x + o_x;
              py = y - font_offset + o_y;
              break;
            case 90:
              px = x + font_offset - o_y;
              py = y + o_x;
              break;
            case 180:
              px = x - o_x;
              py = y + font_offset - o_y;
              break;
            case 270:
              px = x - font_offset + o_y;
              py = y - o_x;
              break;
          }
          rectangle(px, py, scale, scale);
        }
      }

      // Move to the next columns of char and accent data
      d++;
      a++;
    }
  }

  void text(const font_t *font, rect_func rectangle, const std::string_view &t, const int32_t x, const int32_t y, const int32_t wrap, const uint8_t scale, const uint8_t letter_spacing, bool fixed_width, int32_t rotation) {
    uint32_t char_offset = 0;
    uint32_t line_offset = 0; // line (if wrapping) offset
    unicode_sorta::codepage_t codepage = unicode_sorta::PAGE_195;

    int32_t space_width = measure_character(font, ' ', scale, codepage, fixed_width);
    space_width += letter_spacing * scale;

    size_t i = 0;
    while(i < t.length()) {
      // find length of current word
      size_t next_space = t.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = t.length();
      }

      size_t next_linebreak = t.find('\n', i + 1);

      if(next_linebreak == std::string::npos) {
        next_linebreak = t.length();
      }

      size_t next_break = std::min(next_space, next_linebreak);

      uint16_t word_width = 0;
      for(size_t j = i; j < next_break; j++) {
        if (t[j] == unicode_sorta::PAGE_194_START) {
          codepage = unicode_sorta::PAGE_194;
          continue;
        } else if (t[j] == unicode_sorta::PAGE_195_START) {
          continue;
        }
        word_width += measure_character(font, t[j], scale, codepage, fixed_width);
        word_width += letter_spacing * scale;
        codepage = unicode_sorta::PAGE_195;
      }

      // if this word would exceed the wrap limit then
      // move to the next line
      if(char_offset != 0 && char_offset + word_width > (uint32_t)wrap) {
        char_offset = 0;
        line_offset += (font->height + 1) * scale;
      }

      // draw word
      for(size_t j = i; j < std::min(next_break + 1, t.length()); j++) {
        if (t[j] == unicode_sorta::PAGE_194_START) {
          codepage = unicode_sorta::PAGE_194;
          continue;
        } else if (t[j] == unicode_sorta::PAGE_195_START) {
          continue;
        }
        if (t[j] == '\n') { // Linebreak
          line_offset += (font->height + 1) * scale;
          char_offset = 0;
        } else if (t[j] == ' ') { // Space
          char_offset += space_width;
        } else {
          switch(rotation) {
            case 0:
              character(font, rectangle, t[j], x + char_offset, y + line_offset, scale, rotation, codepage);
              break;
            case 90:
              character(font, rectangle, t[j], x - line_offset, y + char_offset, scale, rotation, codepage);
              break;
            case 180:
              character(font, rectangle, t[j], x - char_offset, y - line_offset, scale, rotation, codepage);
              break;
            case 270:
              character(font, rectangle, t[j], x + line_offset, y - char_offset, scale, rotation, codepage);
              break;
          }
          char_offset += measure_character(font, t[j], scale, codepage, fixed_width);
          char_offset += letter_spacing * scale;
        }
        codepage = unicode_sorta::PAGE_195;
      }

      // move character offset
      i = next_break += 1;
    }
  }
}