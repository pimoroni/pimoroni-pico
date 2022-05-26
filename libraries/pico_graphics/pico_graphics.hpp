#pragma once

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>
#include "libraries/bitmap_fonts/font6_data.hpp"

// a tiny little graphics library for our Pico products
// supports only 16-bit (565) RGB framebuffers
namespace pimoroni {

  typedef uint8_t Pen;

  struct Rect;

  struct Point {
    int32_t x = 0, y = 0;

    Point() = default;
    Point(int32_t x, int32_t y) : x(x), y(y) {}

    inline Point& operator-= (const Point &a) { x -= a.x; y -= a.y; return *this; }
    inline Point& operator+= (const Point &a) { x += a.x; y += a.y; return *this; }

    Point clamp(const Rect &r) const;
  };

  struct Rect {
    int32_t x = 0, y = 0, w = 0, h = 0;

    Rect() = default;
    Rect(int32_t x, int32_t y, int32_t w, int32_t h) : x(x), y(y), w(w), h(h) {}
    Rect(const Point &tl, const Point &br) : x(tl.x), y(tl.y), w(br.x - tl.x), h(br.y - tl.y) {}

    bool empty() const;
    bool contains(const Point &p) const;
    bool contains(const Rect &p) const;
    bool intersects(const Rect &r) const;
    Rect intersection(const Rect &r) const;

    void inflate(int32_t v);
    void deflate(int32_t v);
  };

  class PicoGraphics {
  public:
    Pen *frame_buffer;

    Rect      bounds;
    Rect      clip;

    Pen       pen;

    const bitmap::font_t *font;

    uint16_t palette[256];
    uint16_t palette_entries = 0;

  public:
    PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer);
    void set_font(const bitmap::font_t *font);
    void set_pen(uint8_t r, uint8_t g, uint8_t b, bool truncate=true);
    void set_pen(Pen p);

    constexpr uint16_t create_pen_rgb565(uint8_t r, uint8_t g, uint8_t b) {
      uint16_t p = ((r & 0b11111000) << 8) |
                   ((g & 0b11111100) << 3) |
                   ((b & 0b11111000) >> 3);

      return __builtin_bswap16(p);
    }
  
    constexpr uint16_t create_pen_rgb332(uint8_t r, uint8_t g, uint8_t b) {
      uint16_t p = ((r & 0b11100000) << 8) |
                   ((g & 0b11100000) << 3) |
                   ((b & 0b11000000) >> 3);

      return __builtin_bswap16(p);
    }

    Pen create_pen(uint8_t r, uint8_t g, uint8_t b, bool truncate=true) {
      return put_palette(truncate ? create_pen_rgb332(r, g, b) : create_pen_rgb565(r, g, b));
    }

    void flush_palette() {
      palette_entries = 0;
    }

    void default_palette() {
      for (auto i = 0u; i < 255; i++) {
        palette[i] = ((i & 0b11100000) << 8) | ((i & 0b00011100) << 6) | ((i & 0b00000011) << 3);
        palette[i] = __builtin_bswap16(palette[i]);
      }
      palette_entries = 255;
    }

    uint8_t put_palette(uint16_t p);
    uint16_t get_palette(uint8_t i);
    void put_palette(uint16_t p, uint8_t i);

    void set_clip(const Rect &r);
    void remove_clip();

    Pen* ptr(const Point &p);
    Pen* ptr(const Rect &r);
    Pen* ptr(int32_t x, int32_t y);

    void clear();
    void pixel(const Point &p);
    void pixel_span(const Point &p, int32_t l);
    void rectangle(const Rect &r);
    void circle(const Point &p, int32_t r);
    void character(const char c, const Point &p, uint8_t scale = 2);
    void text(const std::string &t, const Point &p, int32_t wrap, uint8_t scale = 2);
    int32_t measure_text(const std::string &t, uint8_t scale = 2);
    void polygon(const std::vector<Point> &points);
    void triangle(Point p1, Point p2, Point p3);
    void line(Point p1, Point p2);
  };

}
