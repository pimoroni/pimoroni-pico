#include <map>
#include <string>
#include <functional>
#include <cstdint>

namespace hershey {
  struct font_glyph_t {
    uint32_t width;        // character width
    uint32_t vertex_count; // number of vertices
    const int8_t  *vertices;     // vertex data (indices: even = x, odd = y)
  };

  struct font_t {
    font_glyph_t chars[95];
  };

  extern const int8_t futural_vertices[2442];
  extern const font_t futural;

  extern const int8_t futuram_vertices[4802];
  extern const font_t futuram;

  extern const int8_t gothgbt_vertices[9046];
  extern const font_t gothgbt;

  extern const int8_t scriptc_vertices[5530];
  extern const font_t scriptc;

  extern const int8_t scripts_vertices[4472];
  extern const font_t scripts;

  extern const int8_t timesi_vertices[4848];
  extern const font_t timesi;

  extern const int8_t timesr_vertices[4600];
  extern const font_t timesr;

  extern const int8_t timesrb_vertices[7994];
  extern const font_t timesrb;

  typedef std::function<void(int32_t x1, int32_t y1, int32_t x2, int32_t y2)> line_func;

  extern std::map<std::string, const font_t*> fonts;

  inline float deg2rad(float degrees);
  const font_glyph_t* glyph_data(const font_t* font, unsigned char c);
  int32_t measure_glyph(const font_t* font, unsigned char c, float s);
  int32_t measure_text(const font_t* font, std::string_view message, float s);
  int32_t glyph(const font_t* font, line_func line, unsigned char c, int32_t x, int32_t y, float s, float a);
  void text(const font_t* font, line_func line, std::string_view message, int32_t x, int32_t y, float s, float a);

  bool has_font(std::string_view font);
  const font_t* font(std::string_view font);
}
