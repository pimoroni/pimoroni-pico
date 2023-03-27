#include "hershey_fonts.hpp"
#include "common/unicode_sorta.hpp"
#include <cmath>

namespace hershey {
#ifndef MICROPY_BUILD_TYPE
  std::map<std::string, const font_t*> fonts = {
    { "sans",         &futural },
    //{ "sans_bold",    &futuram },
    { "gothic",       &gothgbt },
    //{ "cursive_bold", &scriptc },
    { "cursive",      &scripts },
    { "serif_italic", &timesi  },
    { "serif",        &timesr  },
    //{ "serif_bold",   &timesrb }
  };
#endif

  bool has_font(std::string_view font) {
    if(font == "sans"
    || font == "gothic"
    || font == "cursive"
    || font == "serif_italic"
    || font == "serif") {
      return true;
    }
    return false;
  }

  const font_t* font(std::string_view font) {
    if(font == "sans") return &futural;
    else if(font == "gothic") return &gothgbt;
    else if(font == "cursive") return &scripts;
    else if(font == "serif_italic") return &timesi;
    else if(font == "serif") return &timesr;
    return &futural;
  }

  inline float deg2rad(float degrees) {
    return (degrees * M_PI) / 180.0f;
  }

  const font_glyph_t* glyph_data(const font_t* font, unsigned char c) {
    if(c < 32 || c > 127 + 64) { // + 64 char remappings defined in unicode_sorta.hpp
      return nullptr;
    }

    if(c > 127) {
      c = unicode_sorta::char_base_195[c - 128];
    }

    return &font->chars[c - 32];
  }

  int32_t measure_glyph(const font_t* font, unsigned char c, float s) {
    const font_glyph_t *gd = glyph_data(font, c);

    // if glyph data not found (id too great) then skip
    if(!gd) {
      return 0;
    }

    return gd->width * s;
  }

  int32_t measure_text(const font_t* font, std::string_view message, float s) {
    int32_t width = 0;
    for(auto &c : message) {
      width += measure_glyph(font, c, s);
    }
    return width;
  }

  int32_t glyph(const font_t* font, line_func line, unsigned char c, int32_t x, int32_t y, float s, float a) {
    const font_glyph_t *gd = glyph_data(font, c);

    // if glyph data not found (id too great) then skip
    if(!gd) {
      return 0;
    }

    a = deg2rad(a);
    float as = sin(a);
    float ac = cos(a);

    const int8_t *pv = gd->vertices;
    int8_t cx = (*pv++) * s;
    int8_t cy = (*pv++) * s;
    bool pen_down = true;

    for(uint32_t i = 1; i < gd->vertex_count; i++) {
      if(pv[0] == -128 && pv[1] == -128) {
        pen_down = false;
        pv += 2;
      }else{
        int8_t nx = (*pv++) * s;
        int8_t ny = (*pv++) * s;

        int rcx = (cx * ac - cy * as) + 0.5f;
        int rcy = (cx * as + cy * ac) + 0.5f;

        int rnx = (nx * ac - ny * as) + 0.5f;
        int rny = (nx * as + ny * ac) + 0.5f;

        if(pen_down) {
          line(rcx + x, rcy + y, rnx + x, rny + y);
        }

        cx = nx;
        cy = ny;
        pen_down = true;
      }
    }

    return gd->width * s;
  }

  void text(const font_t* font, line_func line, std::string_view message, int32_t x, int32_t y, float s, float a) {
    int32_t cx = x;
    int32_t cy = y;

    int32_t ox = 0;

    float as = sin(deg2rad(a));
    float ac = cos(deg2rad(a));

    for(auto &c : message) {
      int rcx = (ox * ac) + 0.5f;
      int rcy = (ox * as) + 0.5f;

      ox += glyph(font, line, c, cx + rcx, cy + rcy, s, a);
    }
  }
}