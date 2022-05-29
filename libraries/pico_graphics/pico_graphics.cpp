#include "pico_graphics.hpp"

namespace pimoroni {
  PicoGraphics::PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer)
  : frame_buffer((Pen *)frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {
    set_font(&font6);
    set_palette_mode(PaletteModeRGB332);
  };

  void PicoGraphics::set_font(const bitmap::font_t *font){
    this->font = font;
  }

  void PicoGraphics::set_pen(Pen p) {
    pen = p;
  }

  void PicoGraphics::set_palette_mode(PaletteMode mode) {
    palette_mode = mode;
    if(mode == PaletteModeRGB332) {
      rgb332_palette();
    } else {
      empty_palette();
    }
  }

  int PicoGraphics::create_pen(uint8_t r, uint8_t g, uint8_t b) {
    if (palette_mode == PaletteModeRGB332) {
      return rgb_to_rgb332_index(r, g, b); // Fast pack RGB into palette index
    } else {
      RGB565 c = create_pen_rgb565(r, g, b);
      int result = search_palette(c);

      if (result == -1) {
        result = put_palette(create_pen_rgb565(r, g, b));
      }

      return result;
    }
  }

  void PicoGraphics::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    int result = create_pen(r, g, b);
    (void)result;
  }

  int PicoGraphics::search_palette(RGB565 c) {
    for(auto i = 0u; i < 256; i++) {
      if((palette_status[i] & PaletteStatusUsed) && palette[i] == c) return i;
    }
    return -1;
  }

  int PicoGraphics::put_palette(RGB565 c) {
    if(palette_mode != PaletteModeUSER) return -1;

    for(auto i = 0u; i < 256; i++) {
      if(!(palette_status[i] & (PaletteStatusUsed | PaletteStatusReserved))) {
        palette[i] = c;
        palette_status[i] = PaletteStatusUsed;
        return i;
      }
    }
    return -1;
  }

  void PicoGraphics::set_palette(uint8_t i, RGB565 c) {
    if(palette_mode != PaletteModeUSER) return;

    palette[i] = c;
    palette_status[i] |= PaletteStatusUsed;
  }

  int PicoGraphics::reserve_palette() {
    if(palette_mode != PaletteModeUSER) return - 1;

    for (auto i = 0u; i < 256; i++) {
      if (!palette_status[i]) {
        palette_status[i] = PaletteStatusReserved;
        return i;
      }
    }
    return -1;
  }

  void PicoGraphics::empty_palette() {
    for (auto i = 0u; i < 256; i++) {
      palette[i] = 0;
      palette_status[i] = 0;
    }
  }

  void PicoGraphics::rgb332_palette() {
    for (auto i = 0u; i < 256; i++) {
      // Convert the implicit RGB332 (i) into RGB565
      // 0b11100 000 0b00011100 0b00000011
      palette[i] = ((i & 0b11100000) << 8) | ((i & 0b00011100) << 6) | ((i & 0b00000011) << 3);
      palette[i] = __builtin_bswap16(palette[i]);
      palette_status[i] = PaletteStatusUsed;
    }
  }

  void PicoGraphics::set_clip(const Rect &r) {
    clip = bounds.intersection(r);
  }

  void PicoGraphics::remove_clip() {
    clip = bounds;
  }

  Pen* PicoGraphics::ptr(const Rect &r) {
    return frame_buffer + r.x + r.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(const Point &p) {
    return frame_buffer + p.x + p.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(int32_t x, int32_t y) {
    return frame_buffer + x + y * bounds.w;
  }

  void PicoGraphics::clear() {
    rectangle(clip);
  }

  void PicoGraphics::pixel(const Point &p) {
    if(!clip.contains(p)) return;
    *ptr(p) = pen;
  }

  void PicoGraphics::pixel_span(const Point &p, int32_t l) {
    // check if span in bounds
    if( p.x + l < clip.x || p.x >= clip.x + clip.w ||
        p.y     < clip.y || p.y >= clip.y + clip.h) return;

    // clamp span horizontally
    Point clipped = p;
    if(clipped.x     <  clip.x)           {l += clipped.x - clip.x; clipped.x = clip.x;}
    if(clipped.x + l >= clip.x + clip.w)  {l  = clip.x + clip.w - clipped.x;}

    Pen *dest = ptr(clipped);
    while(l--) {
      *dest++ = pen;
    }
  }

  void PicoGraphics::rectangle(const Rect &r) {
    // clip and/or discard depending on rectangle visibility
    Rect clipped = r.intersection(clip);

    if(clipped.empty()) return;

    Pen *dest = ptr(clipped);
    while(clipped.h--) {
      // draw span of pixels for this row
      for(int32_t i = 0; i < clipped.w; i++) {
        *dest++ = pen;
      }

      // move to next scanline
      dest += bounds.w - clipped.w;
    }
  }

  void PicoGraphics::circle(const Point &p, int32_t radius) {
    // circle in screen bounds?
    Rect bounds = Rect(p.x - radius, p.y - radius, radius * 2, radius * 2);
    if(!bounds.intersects(clip)) return;

    int ox = radius, oy = 0, err = -radius;
    while (ox >= oy)
    {
      int last_oy = oy;

      err += oy; oy++; err += oy;

      pixel_span(Point(p.x - ox, p.y + last_oy), ox * 2 + 1);
      if (last_oy != 0) {
        pixel_span(Point(p.x - ox, p.y - last_oy), ox * 2 + 1);
      }

      if(err >= 0 && ox != last_oy) {
        pixel_span(Point(p.x - last_oy, p.y + ox), last_oy * 2 + 1);
        if (ox != 0) {
          pixel_span(Point(p.x - last_oy, p.y - ox), last_oy * 2 + 1);
        }

        err -= ox; ox--; err -= ox;
      }
    }
  }

  void PicoGraphics::character(const char c, const Point &p, uint8_t scale) {
    bitmap::character(font, [this](int32_t x, int32_t y, int32_t w, int32_t h){
      rectangle(Rect(x, y, w, h));
    }, c, p.x, p.y, scale);
  }

  void PicoGraphics::text(const std::string &t, const Point &p, int32_t wrap, uint8_t scale) {
    bitmap::text(font, [this](int32_t x, int32_t y, int32_t w, int32_t h){
      rectangle(Rect(x, y, w, h));
    }, t, p.x, p.y, wrap, scale);
  }

  int32_t PicoGraphics::measure_text(const std::string &t, uint8_t scale) {
    return bitmap::measure_text(font, t, scale);
  }

  int32_t orient2d(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
  }

  bool is_top_left(const Point &p1, const Point &p2) {
    return (p1.y == p2.y && p1.x > p2.x) || (p1.y < p2.y);
  }

  void PicoGraphics::triangle(Point p1, Point p2, Point p3) {
    Rect triangle_bounds(
      Point(std::min(p1.x, std::min(p2.x, p3.x)), std::min(p1.y, std::min(p2.y, p3.y))),
      Point(std::max(p1.x, std::max(p2.x, p3.x)), std::max(p1.y, std::max(p2.y, p3.y))));

    // clip extremes to frame buffer size
    triangle_bounds = clip.intersection(triangle_bounds);

    // if triangle completely out of bounds then don't bother!
    if (triangle_bounds.empty()) {
      return;
    }

    // fix "winding" of vertices if needed
    int32_t winding = orient2d(p1, p2, p3);
    if (winding < 0) {
      Point t;
      t = p1; p1 = p3; p3 = t;
    }

    // bias ensures no overdraw between neighbouring triangles
    int8_t bias0 = is_top_left(p2, p3) ? 0 : -1;
    int8_t bias1 = is_top_left(p3, p1) ? 0 : -1;
    int8_t bias2 = is_top_left(p1, p2) ? 0 : -1;

    int32_t a01 = p1.y - p2.y;
    int32_t b01 = p2.x - p1.x;
    int32_t a12 = p2.y - p3.y;
    int32_t b12 = p3.x - p2.x;
    int32_t a20 = p3.y - p1.y;
    int32_t b20 = p1.x - p3.x;

    Point tl(triangle_bounds.x, triangle_bounds.y);
    int32_t w0row = orient2d(p2, p3, tl) + bias0;
    int32_t w1row = orient2d(p3, p1, tl) + bias1;
    int32_t w2row = orient2d(p1, p2, tl) + bias2;

    for (int32_t y = 0; y < triangle_bounds.h; y++) {
      int32_t w0 = w0row;
      int32_t w1 = w1row;
      int32_t w2 = w2row;

      Pen *dest = ptr(triangle_bounds.x, triangle_bounds.y + y);
      for (int32_t x = 0; x < triangle_bounds.w; x++) {
        if ((w0 | w1 | w2) >= 0) {
          *dest = pen;
        }

        dest++;

        w0 += a12;
        w1 += a20;
        w2 += a01;
      }

      w0row += b12;
      w1row += b20;
      w2row += b01;
    }
  }

  void PicoGraphics::polygon(const std::vector<Point> &points) {
    static int32_t nodes[64]; // maximum allowed number of nodes per scanline for polygon rendering

    int32_t miny = points[0].y, maxy = points[0].y;

    for (uint16_t i = 1; i < points.size(); i++) {
      miny = std::min(miny, points[i].y);
      maxy = std::max(maxy, points[i].y);
    }

    // for each scanline within the polygon bounds (clipped to clip rect)
    Point p;

    for (p.y = std::max(clip.y, miny); p.y <= std::min(clip.y + clip.h, maxy); p.y++) {
      uint8_t n = 0;
      for (uint16_t i = 0; i < points.size(); i++) {
        uint16_t j = (i + 1) % points.size();
        int32_t sy = points[i].y;
        int32_t ey = points[j].y;
        int32_t fy = p.y;
        if ((sy < fy && ey >= fy) || (ey < fy && sy >= fy)) {
          int32_t sx = points[i].x;
          int32_t ex = points[j].x;
          int32_t px = int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx));

          nodes[n++] = px < clip.x ? clip.x : (px >= clip.x + clip.w ? clip.x + clip.w - 1 : px);// clamp(int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx)), clip.x, clip.x + clip.w);
        }
      }

      uint16_t i = 0;
      while (i < n - 1) {
        if (nodes[i] > nodes[i + 1]) {
          int32_t s = nodes[i]; nodes[i] = nodes[i + 1]; nodes[i + 1] = s;
          if (i) i--;
        }
        else {
          i++;
        }
      }

      for (uint16_t i = 0; i < n; i += 2) {
        pixel_span(Point(nodes[i], p.y), nodes[i + 1] - nodes[i] + 1);
      }
    }
  }

  void PicoGraphics::line(Point p1, Point p2) {
    // fast horizontal line
    if(p1.y == p2.y) {
      int32_t start = std::max(clip.x, std::min(p1.x, p2.x));
      int32_t end   = std::min(clip.x + clip.w, std::max(p1.x, p2.x));
      pixel_span(Point(start, p1.y), end - start);
      return;
    }

    // fast vertical line
    if(p1.x == p2.x) {
      int32_t start  = std::max(clip.y, std::min(p1.y, p2.y));
      int32_t length = std::min(clip.y + clip.h, std::max(p1.y, p2.y)) - start;
      Pen *dest = ptr(p1.x, start);
      while(length--) {
        *dest = pen;
        dest += bounds.w;
      }
      return;
    }

    // general purpose line
    // lines are either "shallow" or "steep" based on whether the x delta
    // is greater than the y delta
    int32_t dx = p2.x - p1.x;
    int32_t dy = p2.y - p1.y;
    bool shallow = std::abs(dx) > std::abs(dy);
    if(shallow) {
      // shallow version
      int32_t s = std::abs(dx);       // number of steps
      int32_t sx = dx < 0 ? -1 : 1;   // x step value
      int32_t sy = (dy << 16) / s;    // y step value in fixed 16:16
      int32_t x = p1.x;
      int32_t y = p1.y << 16;
      while(s--) {
        pixel(Point(x, y >> 16));
        y += sy;
        x += sx;
      }
    }else{
      // steep version
      int32_t s = std::abs(dy);       // number of steps
      int32_t sy = dy < 0 ? -1 : 1;   // y step value
      int32_t sx = (dx << 16) / s;    // x step value in fixed 16:16
      int32_t y = p1.y;
      int32_t x = p1.x << 16;
      while(s--) {
        pixel(Point(x >> 16, y));
        y += sy;
        x += sx;
      }
    }
  }
}
