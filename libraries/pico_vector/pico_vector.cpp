#include "pico_vector.hpp"
#include <vector>

namespace pimoroni {
  void PicoVector::polygon(std::vector<pretty_poly::contour_t<int>> contours, Point origin, int scale) {
    pretty_poly::draw_polygon<int>(
        contours,
        pretty_poly::point_t<int>(origin.x, origin.y),
        scale);
  }

  Point PicoVector::text(std::string_view text, Point origin) {
    // TODO: Normalize types somehow, so we're not converting?
    pretty_poly::point_t<int> caret = pretty_poly::point_t<int>(origin.x, origin.y);

    // Align text from the bottom left
    caret.y += text_metrics.size;

    int16_t space_width = alright_fonts::measure_character(text_metrics, ' ').w;
    if (space_width == 0) {
      space_width = text_metrics.word_spacing;
    }

    size_t i = 0;

    while(i < text.length()) {
      size_t next_space = text.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = text.length();
      }

      size_t next_linebreak = text.find('\n', i + 1);

      if(next_linebreak == std::string::npos) {
        next_linebreak = text.length();
      }

      size_t next_break = std::min(next_space, next_linebreak);

      uint16_t word_width = 0;
      for(size_t j = i; j < next_break; j++) {
        word_width += alright_fonts::measure_character(text_metrics, text[j]).w;
        word_width += text_metrics.letter_spacing;
      }

      if(caret.x != 0 && caret.x + word_width > graphics->clip.w) {
        caret.x = origin.x;
        caret.y += text_metrics.line_height;
      }

      for(size_t j = i; j < std::min(next_break + 1, text.length()); j++) {
        if (text[j] == '\n') { // Linebreak
          caret.x = origin.x;
          caret.y += text_metrics.line_height;
        } else if (text[j] == ' ') { // Space
          caret.x += space_width;
        } else {
          alright_fonts::render_character(text_metrics, text[j], caret);
        }
        caret.x += alright_fonts::measure_character(text_metrics, text[j]).w;
        caret.x += text_metrics.letter_spacing;
      }

      i = next_break + 1;
    }

    return Point(caret.x, caret.y);
  }
}