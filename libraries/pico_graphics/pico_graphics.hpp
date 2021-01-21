#pragma once

#include <string>
#include <cstdint>
#include <algorithm>

// a tiny little graphics library for our Pico products
// supports only 16-bit (565) RGB framebuffers
namespace pimoroni {

  struct rect;

  struct point {
    int32_t x = 0, y = 0;

    point() = default;
    point(int32_t x, int32_t y) : x(x), y(y) {}

    inline point& operator-= (const point &a) { x -= a.x; y -= a.y; return *this; }
    inline point& operator+= (const point &a) { x += a.x; y += a.y; return *this; }

    point clamp(const rect &r) const;
  };

  struct rect {
    int32_t x = 0, y = 0, w = 0, h = 0;

    rect() = default;
    rect(int32_t x, int32_t y, int32_t w, int32_t h) : x(x), y(y), w(w), h(h) {}

    bool empty() const;
    bool contains(const point &p) const;
    bool contains(const rect &p) const;
    bool intersects(const rect &r) const;
    rect intersection(const rect &r) const;

    void inflate(int32_t v);
    void deflate(int32_t v);
  };

  class PicoGraphics {
  public:
    uint16_t *frame_buffer;

    rect      bounds;
    rect      clip;

    uint16_t  pen;

  public:
    PicoGraphics(uint16_t width, uint16_t height, uint16_t *frame_buffer)
      : frame_buffer(frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {}

    void set_pen(uint8_t r, uint8_t g, uint8_t b);
    void set_pen(uint16_t p);
    uint16_t create_pen(uint8_t r, uint8_t g, uint8_t b);

    void set_clip(const rect &r);
    void remove_clip();

    uint16_t* ptr(const point &p);
    uint16_t* ptr(const rect &r);
    uint16_t* ptr(int32_t x, int32_t y);

    void clear();
    void pixel(const point &p);
    void pixel_span(const point &p, int32_t l);
    void rectangle(const rect &r);
    void circle(const point &p, int32_t r);
    void character(const char c, const point &p, uint8_t scale = 2);
    void text(const std::string &t, const point &p, int32_t wrap, uint8_t scale = 2);
    //void polygon(std::vector);
  };

}