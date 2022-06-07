#pragma once

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>
#include "libraries/bitmap_fonts/font6_data.hpp"

// a tiny little graphics library for our Pico products
// supports only 16-bit (565) RGB framebuffers
namespace pimoroni {
  typedef uint8_t RGB332;
  typedef uint16_t RGB565;
  typedef int Pen;

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

  class PicoGraphicsPenType {
      public:

        struct PaletteEntry {
          RGB565 color;
          bool used;
        };

        static constexpr RGB332 rgb_to_rgb332(uint8_t r, uint8_t g, uint8_t b) {
          return (r & 0b11100000) | ((g & 0b11100000) >> 3) | ((b & 0b11000000) >> 6);
        }

        static constexpr RGB565 rgb332_to_rgb565(RGB332 c) {
          uint16_t p = ((c & 0b11100000) << 8) |
                       ((c & 0b00011100) << 6) |
                       ((c & 0b00000011) << 3);
          return __builtin_bswap16(p);
        }

        static constexpr RGB565 rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
          uint16_t p = ((r & 0b11111000) << 8) |
                       ((g & 0b11111100) << 3) |
                       ((b & 0b11111000) >> 3);

          return __builtin_bswap16(p);
        }

        virtual void set_pixel(void *frame_buffer, uint x, uint y, uint stride);
        virtual int create(uint8_t r, uint8_t g, uint8_t b);
        virtual void set_color(uint c);
        virtual void set_color(uint8_t r, uint8_t g, uint8_t b);
        virtual void update_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b);

        virtual void palette_lookup(void *frame_buffer, void *result, uint offset, uint length);
        static size_t buffer_size(uint w, uint h); // Must be static, since user must know required size to alloc framebuffer
  };

  class PenP4 : public PicoGraphicsPenType {
    public:
      uint8_t color;
      PaletteEntry palette[8];
      PenP4() {
        palette[0].color = rgb_to_rgb565(57, 48, 57);     // Black
        palette[1].color = rgb_to_rgb565(255, 255, 255);  // White
        palette[2].color = rgb_to_rgb565(58, 91, 70);     // Green
        palette[3].color = rgb_to_rgb565(61, 59, 94);     // Blue
        palette[4].color = rgb_to_rgb565(156, 72, 75);    // Red
        palette[5].color = rgb_to_rgb565(208, 190, 71);   // Yellow
        palette[6].color = rgb_to_rgb565(177, 106, 73);   // Orange
        palette[7].color = rgb_to_rgb565(255, 255, 255);  // Clear
      }
      void set_color(uint c) {
        color = c & 0xf;
      }
      void set_color(uint8_t r, uint8_t g, uint8_t b) override {
        // TODO look up closest palette colour, or just NOOP?
      }
      void update_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {}
      void reset_color(uint8_t i) {}
      int create(uint8_t r, uint8_t g, uint8_t b) override {
        return -1; // Can never create new colours, fixed palette!
      }
      void set_pixel(void *frame_buffer, uint x, uint y, uint stride) override {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        uint8_t *p = &buf[(x / 2) + (y * stride / 2)];

        uint8_t  o = (~x & 0b1) * 4; // bit offset within byte
        uint8_t  m = ~(0b1111 << o); // bit mask for byte
        uint8_t  b = color << o;     // bit value shifted to position

        *p &= m; // clear bits
        *p |= b; // set value
      }
      void palette_lookup(void *frame_buffer, void *result, uint offset, uint length) override {
        uint8_t *src = (uint8_t *)frame_buffer;
        uint16_t *dst = (uint16_t *)result;
        for(auto x = 0u; x < length; x++) {
          uint8_t c = src[(offset / 2) + (x / 2)];
          uint8_t  o = (~x & 0b1) * 4; // bit offset within byte
          uint8_t  b = (c >> o) & 0xf; // bit value shifted to position
          dst[x] = palette[b].color;
        }
      }
      static size_t buffer_size(uint w, uint h) {
        return w * h / 2;
      }
  };

  class PenP8 : public PicoGraphicsPenType {
    public:
      uint8_t color;
      PaletteEntry palette[256];
      PenP8() {
        for(auto i = 0u; i < 256; i++) {
          reset_color(i);
        }
      }
      void set_color(uint c) override {
        color = c;
      }
      void set_color(uint8_t r, uint8_t g, uint8_t b) override {
        // TODO look up closest palette colour, or just NOOP?
      }
      void update_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
        palette[i].color = rgb_to_rgb565(r, g, b);
        palette[i].used = true;
      }
      void reset_color(uint8_t i) {
        palette[i].color = 0;
        palette[i].used = false;
      }
      int create(uint8_t r, uint8_t g, uint8_t b) override {
        // Create a colour and place it in the palette if there's space
        RGB565 c = rgb_to_rgb565(r, g, b);
        for(auto i = 0u; i < 256u; i++) {
          if(!palette[i].used) {
            palette[i].color = c;
            palette[i].used = true;
            return i;
          }
        }
        return -1;
      }
      void set_pixel(void *frame_buffer, uint x, uint y, uint stride) override {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[y * stride + x] = color;
      }
      void palette_lookup(void *frame_buffer, void *result, uint offset, uint length) override {
        uint8_t *src = (uint8_t *)frame_buffer;
        uint16_t *dst = (uint16_t *)result;
        for(auto x = 0u; x < length; x++) {
          dst[x] = palette[src[offset + x]].color;
        }
      }
      static size_t buffer_size(uint w, uint h) {
        return w * h;
      }
  };

  class PenRGB332 : public PicoGraphicsPenType {
    public:
      RGB332 color;
      PaletteEntry palette[256];
      PenRGB332() {
        for(auto i = 0u; i < 256; i++) {
          reset_color(i);
        }
      }
      void set_color(uint c) {
        color = c;
      }
      void set_color(uint8_t r, uint8_t g, uint8_t b) override {
        color = rgb_to_rgb332(r, g, b);
      }
      void update_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
        palette[i].color = rgb_to_rgb565(r, g, b);
        palette[i].used = true;
      }
      void reset_color(uint8_t i) {
        palette[i].color = rgb332_to_rgb565(i);
        palette[i].used = true;
      }
      int create(uint8_t r, uint8_t g, uint8_t b) override {
        return rgb_to_rgb332(r, g, b);
      }
      void set_pixel(void *frame_buffer, uint x, uint y, uint stride) override {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[y * stride + x] = color;
      }
      void palette_lookup(void *frame_buffer, void *result, uint offset, uint length) override {
        uint8_t *src = (uint8_t *)frame_buffer;
        uint16_t *dst = (uint16_t *)result;
        for(auto x = 0u; x < length; x++) {
          dst[x] = palette[src[offset + x]].color;
        }
      }
      static size_t buffer_size(uint w, uint h) {
        return w * h;
      }
  };

  class PenRGB565 : public PicoGraphicsPenType {
    public:
      uint16_t color;
      void set_color(uint c) override {
        color = c;
      }
      void update_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
        // Palette only. Does nothing.
      }
      void reset_color(uint8_t i) {
        // Palette only. Does nothing.
      }
      void set_color(uint8_t r, uint8_t g, uint8_t b) override {
        color = rgb_to_rgb565(r, g, b);
      }
      int create(uint8_t r, uint8_t g, uint8_t b) override {
        return rgb_to_rgb565(r, g, b);
      }
      void set_pixel(void *frame_buffer, uint x, uint y, uint stride) override {
        uint16_t *buf = (uint16_t *)frame_buffer;
        buf[y * stride + x] = color;
      }
      void palette_lookup(void *frame_buffer, void *result, uint offset, uint length) override {
      }
      static size_t buffer_size(uint w, uint h) {
        return w * h * sizeof(RGB565);
      }
  };

  template <class T=PicoGraphicsPenType>
  class PicoGraphics {
  public:
    void *frame_buffer;

    Rect bounds;
    Rect clip;

    const bitmap::font_t *font;

    T pen;

  public:
    PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer)
    : frame_buffer(frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {
      set_font(&font6);
      if(this->frame_buffer == nullptr) {
        this->frame_buffer = (void *)(new uint8_t[pen.buffer_size(width, height)]);
      }
    };

    void set_font(const bitmap::font_t *font);
    void set_pen(uint16_t p);

    int create_pen(uint8_t r, uint8_t g, uint8_t b) {
      return pen.create(r, g, b);
    }

    void set_pen(uint8_t r, uint8_t g, uint8_t b) {
      pen.set_color(r, g, b);
    }

    void update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
      pen.update_color(i, r, g, b);
    }

    void reset_pen(uint8_t i) {
      pen.reset_color(i);
    }

    void set_dimensions(int width, int height) {
      bounds.w = width;
      bounds.h = height;
      clip.w = width;
      clip.h = height;
    }

    void set_clip(const Rect &r);
    void remove_clip();

    void *get_data() {
      return frame_buffer;
    }

    void get_data(uint y, void *row_buf) {
      pen.palette_lookup(frame_buffer, row_buf, y * bounds.w, bounds.w);
    }

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

  template class PicoGraphics<PenP4>;
  template class PicoGraphics<PenP8>;
  template class PicoGraphics<PenRGB332>;
  template class PicoGraphics<PenRGB565>;

}
