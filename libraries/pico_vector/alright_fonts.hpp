#include <cstdint>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <optional>
#include <map>

#include "pretty_poly.hpp"

namespace alright_fonts {

  struct glyph_t {
    uint16_t codepoint;
    pretty_poly::rect_t bounds;
    uint8_t advance;
    std::vector<pretty_poly::contour_t<int8_t>> contours;
  };

  struct face_t {
    uint16_t glyph_count;
    uint16_t flags;
    std::map<uint16_t, glyph_t> glyphs;

    face_t() {};
    face_t(pretty_poly::file_io &ifs) {load(ifs);}
    face_t(std::string_view path) {load(path);}

    bool load(pretty_poly::file_io &ifs);
    bool load(std::string_view path);
  };

  enum alignment_t {
    left    = 0,
    center  = 1,
    right   = 2,
    justify = 4,
    top     = 8,
    bottom  = 16
  };

  struct text_metrics_t {
    face_t face;                      // font to write in
    int size;                         // text size in pixels
    uint scroll;                      // vertical scroll offset
    int line_height;                  // spacing between lines (%)
    int letter_spacing;               // spacing between characters
    int word_spacing;                 // spacing between words
    alignment_t align;                // horizontal and vertical alignment
    //optional<mat3_t> transform;     // arbitrary transformation
    pretty_poly::antialias_t antialiasing = pretty_poly::X4;    // level of antialiasing to apply

    void set_size(int s) {
      size = s;
      line_height = size;
      letter_spacing = 0;
      word_spacing = size / 2;
    }

    text_metrics_t() {};
  };

  /*
    utility functions
  */
  pretty_poly::rect_t measure_character(text_metrics_t &tm, uint16_t codepoint);

  /* 
    render functions
  */

  void render_character(text_metrics_t &tm, uint16_t codepoint, pretty_poly::point_t<int> origin);
  template<typename mat_t>
  void render_character(text_metrics_t &tm, uint16_t codepoint, pretty_poly::point_t<int> origin, mat_t transform);
}