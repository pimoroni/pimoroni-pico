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
  typedef uint16_t RGB565;

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

    RGB565 palette[256];
    uint8_t palette_status[256];

    enum PaletteMode {
      PaletteModeRGB332 = 0,
      PaletteModeUSER = 1
    };

    enum PaletteStatus : uint8_t {
      PaletteStatusReserved = 1,
      PaletteStatusUsed = 2
    };

    PaletteMode palette_mode = PaletteModeRGB332;

  public:
    PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer);
    void set_font(const bitmap::font_t *font);
    void set_pen(Pen p);
    void set_palette_mode(PaletteMode mode);

    [[deprecated("Use uint8_t create_pen(uint8_t, uint8_t, uint8_t).")]]
    void set_pen(uint8_t r, uint8_t g, uint8_t b);

    static constexpr Pen rgb_to_rgb332_index(uint8_t r, uint8_t g, uint8_t b) {
      return (r & 0b11100000) | ((g & 0b11100000) >> 3) | ((b & 0b11000000) >> 6);
    }

    static constexpr RGB565 create_pen_rgb565(uint8_t r, uint8_t g, uint8_t b) {
      uint16_t p = ((r & 0b11111000) << 8) |
                   ((g & 0b11111100) << 3) |
                   ((b & 0b11111000) >> 3);

      return __builtin_bswap16(p);
    }
  
    static constexpr RGB565 create_pen_rgb332(uint8_t r, uint8_t g, uint8_t b) {
      uint16_t p = ((r & 0b11100000) << 8) |
                   ((g & 0b11100000) << 3) |
                   ((b & 0b11000000) >> 3);

      return __builtin_bswap16(p);
    }

    void set_dimensions(int width, int height) {
      bounds.w = width;
      bounds.h = height;
      clip.w = width;
      clip.h = height;
    }

    int create_pen(uint8_t r, uint8_t g, uint8_t b);

    int reserve_palette();
    void empty_palette();
    void rgb332_palette();

    int search_palette(RGB565 c);
    int get_palette(uint8_t i);
    void set_palette(uint8_t i, RGB565 c);
    int put_palette(RGB565 c);

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
