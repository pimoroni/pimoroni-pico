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

    for (auto c : text) {
        alright_fonts::render_character(text_metrics, c, caret);
        if(text_metrics.face.glyphs.count(c) == 1) {
            alright_fonts::glyph_t glyph = text_metrics.face.glyphs[c];
            caret.x += ((glyph.advance * text_metrics.size) / 128);
            if(caret.x > graphics->bounds.w) {
                caret.y += text_metrics.size;
                caret.x = origin.x;
            }
        }
    }

    return Point(caret.x, caret.y);
  }
}