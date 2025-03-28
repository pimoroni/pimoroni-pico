#include <cstdint>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <optional>
#include <map>

#include "alright_fonts.hpp"

namespace alright_fonts {
  /*
    utility functions
  */
  pp_rect_t measure_character(text_metrics_t &tm, uint16_t codepoint) {
    if(tm.face.glyphs.count(codepoint) == 1) {
      glyph_t glyph = tm.face.glyphs[codepoint];

      return {0, 0, ((glyph.advance * tm.size) / 128), tm.size};
    }

    return {0, 0, 0, 0};
  }

  /*
    render functions
  */

  void render_character(text_metrics_t &tm, uint16_t codepoint, pp_point_t origin, pp_mat3_t *transform) {
    if(tm.face.glyphs.count(codepoint) == 1) {
      glyph_t glyph = tm.face.glyphs[codepoint];
      pp_transform(transform);
      pp_render(&glyph.contours);
    }
  }
  /*
    load functions
  */

  // big endian stream value helpers
  uint16_t  ru16(file_io &ifs) {uint8_t w[2]; ifs.read((char *)w, 2); return w[0] << 8 | w[1];}
  int16_t   rs16(file_io &ifs) {uint8_t w[2]; ifs.read((char *)w, 2); return w[0] << 8 | w[1];}
  uint32_t  ru32(file_io &ifs) {uint8_t dw[4]; ifs.read((char *)dw, 4); return dw[0] << 24 | dw[1] << 16 | dw[2] << 8 | dw[3];}
  uint8_t   ru8(file_io &ifs) {uint8_t w; ifs.read(&w, 1); return w;}
  int8_t    rs8(file_io &ifs) {int8_t w; ifs.read(&w, 1); return w;}

  bool face_t::load(file_io &ifs) {
    char marker[4];
    ifs.read(marker, sizeof(marker));

    // check header magic bytes are present
    if(memcmp(marker, "af!?", 4) != 0) {
      // doesn't start with magic marker
      return false;
    }

    // number of glyphs embedded in font file
    this->glyph_count = ru16(ifs);

    // extract flags and ensure none set
    this->flags = ru16(ifs);
    if(this->flags != 0) {
      // unknown flags set
      return false;
    }

    // extract glyph dictionary
    uint16_t glyph_entry_size = 9;
    uint32_t contour_data_offset = 8 + this->glyph_count * glyph_entry_size;
    for(auto i = 0; i < this->glyph_count; i++) {
      glyph_t g;
      g.codepoint = ru16(ifs);
      g.bounds.x  = rs8(ifs);
      g.bounds.y  = rs8(ifs);
      g.bounds.w  = ru8(ifs);
      g.bounds.h  = ru8(ifs);
      g.advance   = ru8(ifs);

      g.contours.paths = (pp_path_t *)malloc(sizeof(pp_path_t) * 10);

      if(ifs.fail()) {
        // could not read glyph dictionary entry
        return false;
      }

      // allocate space for the contour data and read it from the font file
      uint16_t contour_data_length = ru16(ifs);

      // remember where we are in the dictionary
      int pos = ifs.tell();

      // read contour data
      ifs.seek(contour_data_offset);
      while(true) {
        // get number of points in contour
        uint16_t count = ru16(ifs);

        // if count is zero then this is the end of contour marker
        if(count == 0) {
          break;
        }

        // allocate space to store point data for contour and read
        // from file
        g.contours.paths[g.contours.count].points = (pp_point_t *)malloc(sizeof(pp_point_t) * count);

        ifs.read((char *)g.contours.paths[g.contours.count].points, sizeof(pp_point_t) * count);

        g.contours.count ++;
      }

      // return back to position in dictionary
      ifs.seek(pos);
      contour_data_offset += contour_data_length;

      if(ifs.fail()) {
        // could not read glyph contour data
        return false;
      }

      this->glyphs[g.codepoint] = g;
    }

    return true;
  }

  bool face_t::load(std::string_view path) {
    file_io ifs(path);
    if(ifs.fail()) {
      // could not open file
      return false;
    }
    return load(ifs);
  }

}