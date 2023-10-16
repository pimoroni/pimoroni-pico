#include "pico_graphics.hpp"

namespace pimoroni {

  const uint8_t dither16_pattern[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

  int PicoGraphics::update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {return -1;};
  int PicoGraphics::reset_pen(uint8_t i) {return -1;};
  int PicoGraphics::create_pen(uint8_t r, uint8_t g, uint8_t b) {return -1;};
  int PicoGraphics::create_pen_hsv(float h, float s, float v){return -1;};
  void PicoGraphics::set_pixel_alpha(const Point &p, const uint8_t a) {};
  void PicoGraphics::set_pixel_dither(const Point &p, const RGB &c) {};
  void PicoGraphics::set_pixel_dither(const Point &p, const RGB565 &c) {};
  void PicoGraphics::set_pixel_dither(const Point &p, const uint8_t &c) {};
  void PicoGraphics::frame_convert(PenType type, conversion_callback_func callback) {};
  void PicoGraphics::sprite(void* data, const Point &sprite, const Point &dest, const int scale, const int transparent) {};

  int PicoGraphics::get_palette_size() {return 0;}
  RGB* PicoGraphics::get_palette() {return nullptr;}
  bool PicoGraphics::supports_alpha_blend() {return false;}

  void PicoGraphics::set_dimensions(int width, int height) {
    bounds = clip = {0, 0, width, height};
  }

  void PicoGraphics::set_framebuffer(void *frame_buffer) {
    this->frame_buffer = frame_buffer;
  }

  void PicoGraphics::set_font(const bitmap::font_t *font){
    this->bitmap_font = font;
    this->hershey_font = nullptr;
  }

  void PicoGraphics::set_font(const hershey::font_t *font){
    this->bitmap_font = nullptr;
    this->hershey_font = font;
  }

  void PicoGraphics::set_font(std::string_view name){
    if (name == "bitmap6") {
      set_font(&font6);
    } else if (name == "bitmap8") {
      set_font(&font8);
    } else if (name == "bitmap14_outline") {
      set_font(&font14_outline);
    } else {
      // check that font exists and assign it
      if(hershey::has_font(name)) {
        set_font(hershey::font(name));
      }
    }
  }

  void PicoGraphics::set_thickness(uint t) {
    thickness = t;
  }

  void PicoGraphics::set_clip(const Rect &r) {
    clip = bounds.intersection(r);
  }

  void PicoGraphics::remove_clip() {
    clip = bounds;
  }
  
  void PicoGraphics::clear() {
    rectangle(clip);
  }

  void PicoGraphics::pixel(const Point &p) {
    if(!clip.contains(p)) return;
    set_pixel(p);
  }

  void PicoGraphics::pixel_span(const Point &p, int32_t l) {
    // check if span in bounds
    if( p.x + l < clip.x || p.x >= clip.x + clip.w ||
        p.y     < clip.y || p.y >= clip.y + clip.h) return;

    // clamp span horizontally
    Point clipped = p;
    if(clipped.x     <  clip.x)           {l += clipped.x - clip.x; clipped.x = clip.x;}
    if(clipped.x + l >= clip.x + clip.w)  {l  = clip.x + clip.w - clipped.x;}

    Point dest(clipped.x, clipped.y);
    set_pixel_span(dest, l);
  }

  void PicoGraphics::rectangle(const Rect &r) {
    // clip and/or discard depending on rectangle visibility
    Rect clipped = r.intersection(clip);

    if(clipped.empty()) return;

    Point dest(clipped.x, clipped.y);
    while(clipped.h--) {
      // draw span of pixels for this row
      set_pixel_span(dest, clipped.w);
      // move to next scanline
      dest.y++;
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

  void PicoGraphics::character(const char c, const Point &p, float s, float a) {
    if (bitmap_font) {
      bitmap::character(bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        rectangle(Rect(x, y, w, h));
      }, c, p.x, p.y, std::max(1.0f, s), int32_t(a) % 360);
      return;
    }

    if (hershey_font) {
      hershey::glyph(hershey_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
        line(Point(x1, y1), Point(x2, y2));
      }, c, p.x, p.y, s, a);
      return;
    }
  }

  void PicoGraphics::text(const std::string_view &t, const Point &p, int32_t wrap, float s, float a, uint8_t letter_spacing, bool fixed_width) {
    if (bitmap_font) {
      bitmap::text(bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        rectangle(Rect(x, y, w, h));
      }, t, p.x, p.y, wrap, std::max(1.0f, s), letter_spacing, fixed_width, int32_t(a) % 360);
      return;
    }

    if (hershey_font) {
      if(thickness == 1) {
        hershey::text(hershey_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
          line(Point(x1, y1), Point(x2, y2));
        }, t, p.x, p.y, s, a);
      } else {
        hershey::text(hershey_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
          thick_line(Point(x1, y1), Point(x2, y2), thickness);
        }, t, p.x, p.y, s, a);
      }
      return;
    }
  }

  int32_t PicoGraphics::measure_text(const std::string_view &t, float s, uint8_t letter_spacing, bool fixed_width) {
    if (bitmap_font) return bitmap::measure_text(bitmap_font, t, std::max(1.0f, s), letter_spacing, fixed_width);
    if (hershey_font) return hershey::measure_text(hershey_font, t, s);
    return 0;
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

      Point dest = Point(triangle_bounds.x, triangle_bounds.y + y);
      for (int32_t x = 0; x < triangle_bounds.w; x++) {
        if ((w0 | w1 | w2) >= 0) {
          set_pixel(dest);
        }

        dest.x++;

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

          nodes[n++] = px;
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

  void PicoGraphics::thick_line(Point p1, Point p2, uint thickness) {
    int32_t ht = thickness / 2;
    int32_t t = (int32_t)thickness;

    // fast horizontal line
    if(p1.y == p2.y) {
      int32_t start = std::min(p1.x, p2.x);
      int32_t end   = std::max(p1.x, p2.x);
      rectangle(Rect(start, p1.y - ht, end - start, t));
      return;
    }

    // fast vertical line
    if(p1.x == p2.x) {
      int32_t start  = std::min(p1.y, p2.y);
      int32_t length = std::max(p1.y, p2.y) - start;
      rectangle(Rect(p1.x - ht, start, t, length));
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
        rectangle({x - ht, (y >> 16) - ht, t, t});
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
        rectangle({(x >> 16) - ht, y - ht, t, t});
        y += sy;
        x += sx;
      }
    }
  }

  void PicoGraphics::line(Point p1, Point p2) {
    // fast horizontal line
    if(p1.y == p2.y) {
      int32_t start = std::min(p1.x, p2.x);
      int32_t end   = std::max(p1.x, p2.x);
      pixel_span(Point(start, p1.y), end - start);
      return;
    }

    // fast vertical line
    if(p1.x == p2.x) {
      int32_t start  = std::min(p1.y, p2.y);
      int32_t length = std::max(p1.y, p2.y) - start;
      Point dest(p1.x, start);
      while(length--) {
        pixel(dest);
        dest.y++;
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
        Point p(x, y >> 16);
        pixel(p);
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
        Point p(x >> 16, y);
        pixel(p);
        y += sy;
        x += sx;
      }
    }
  }

  // Common function for frame buffer conversion to 565 pixel format
  void PicoGraphics::frame_convert_rgb565(conversion_callback_func callback, next_pixel_func get_next_pixel)
  {
    // Allocate two temporary buffers, as the callback may transfer by DMA
    // while we're preparing the next part of the row
    const int BUF_LEN = 64;
    uint16_t row_buf[2][BUF_LEN];
    int buf_idx = 0;
    int buf_entry = 0;
    for(auto i = 0; i < bounds.w * bounds.h; i++) {
      row_buf[buf_idx][buf_entry] = get_next_pixel();
      buf_entry++;

      // Transfer a filled buffer and swap to the next one
      if (buf_entry == BUF_LEN) {
          callback(row_buf[buf_idx], BUF_LEN * sizeof(RGB565));
          buf_idx ^= 1;
          buf_entry = 0;
      }
    }

    // Transfer any remaining pixels ( < BUF_LEN )
    if(buf_entry > 0) {
        callback(row_buf[buf_idx], buf_entry * sizeof(RGB565));
    }

    // Callback with zero length to ensure previous buffer is fully written
    callback(row_buf[buf_idx], 0);
  }

  // Common function for frame buffer conversion to 565 pixel format
  void PicoGraphics::frame_convert_rgb888(conversion_callback_func callback, next_pixel_func_rgb888 get_next_pixel)
  {
    // Allocate two temporary buffers, as the callback may transfer by DMA
    // while we're preparing the next part of the row
    const int BUF_LEN = 64;
    RGB888 row_buf[2][BUF_LEN];
    int buf_idx = 0;
    int buf_entry = 0;
    for(auto i = 0; i < bounds.w * bounds.h; i++) {
      row_buf[buf_idx][buf_entry] = get_next_pixel();
      buf_entry++;

      // Transfer a filled buffer and swap to the next one
      if (buf_entry == BUF_LEN) {
          callback(row_buf[buf_idx], BUF_LEN * sizeof(RGB888));
          buf_idx ^= 1;
          buf_entry = 0;
      }
    }

    // Transfer any remaining pixels ( < BUF_LEN )
    if(buf_entry > 0) {
        callback(row_buf[buf_idx], buf_entry * sizeof(RGB888));
    }

    // Callback with zero length to ensure previous buffer is fully written
    callback(row_buf[buf_idx], 0);
  }
}
