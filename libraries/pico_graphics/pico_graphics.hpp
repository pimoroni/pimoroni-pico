#pragma once

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>

#include "libraries/hershey_fonts/hershey_fonts.hpp"
#include "libraries/bitmap_fonts/bitmap_fonts.hpp"
#include "libraries/bitmap_fonts/font6_data.hpp"
#include "libraries/bitmap_fonts/font8_data.hpp"
#include "libraries/bitmap_fonts/font14_outline_data.hpp"

#include "common/pimoroni_common.hpp"

// A tiny graphics library for our Pico products
// supports:
//   - 16-bit (565) RGB
//   - 8-bit (332) RGB
//   - 8-bit with 16-bit 256 entry palette
//   - 4-bit with 16-bit 8 entry palette
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

  class PicoGraphics {
  public:
    struct PaletteEntry {
      RGB565 color;
      bool used;
    };

    enum PenType {
      PEN_P2 = 0,
      PEN_P4,
      PEN_P8,
      PEN_RGB332,
      PEN_RGB565
    };

    void *frame_buffer;

    PenType pen_type;
    Rect bounds;
    Rect clip;

    const bitmap::font_t *bitmap_font;
    const hershey::font_t *hershey_font;

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

    PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer)
    : frame_buffer(frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {
      set_font(&font6);
    };

    virtual void set_pen(uint c);
    virtual void set_pen(uint8_t r, uint8_t g, uint8_t b);
    virtual int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
    virtual int reset_pen(uint8_t i);
    virtual int create_pen(uint8_t r, uint8_t g, uint8_t b);
    virtual void set_pixel(const Point &p);
    virtual void get_row_rgb565(void *result, uint offset, uint length);

    void set_font(const bitmap::font_t *font);
    void set_font(const hershey::font_t *font);
    void set_font(std::string font);

    void set_dimensions(int width, int height);
    void set_framebuffer(void *frame_buffer);

    void *get_data();
    void get_data(PenType type, uint y, void *row_buf);

    void set_clip(const Rect &r);
    void remove_clip();

    void clear();
    void pixel(const Point &p);
    void pixel_span(const Point &p, int32_t l);
    void rectangle(const Rect &r);
    void circle(const Point &p, int32_t r);
    void character(const char c, const Point &p, float s = 2.0f, float a = 0.0f);
    void text(const std::string &t, const Point &p, int32_t wrap, float s = 2.0f, float a = 0.0f, uint8_t letter_spacing = 1);
    int32_t measure_text(const std::string &t, float s = 2.0f, uint8_t letter_spacing = 1);
    void polygon(const std::vector<Point> &points);
    void triangle(Point p1, Point p2, Point p3);
    void line(Point p1, Point p2);
  };


  class PicoGraphics_PenP4 : public PicoGraphics {
    public:
      uint8_t color;
      PaletteEntry palette[8];
      const RGB565 default_palette[8] = {
        rgb_to_rgb565(57, 48, 57),     // Black
        rgb_to_rgb565(255, 255, 255),  // White
        rgb_to_rgb565(58, 91, 70),     // Green
        rgb_to_rgb565(61, 59, 94),     // Blue
        rgb_to_rgb565(156, 72, 75),    // Red
        rgb_to_rgb565(208, 190, 71),   // Yellow
        rgb_to_rgb565(177, 106, 73),   // Orange
        rgb_to_rgb565(255, 255, 255)   // Clear
      };
      PicoGraphics_PenP4(uint16_t width, uint16_t height, void *frame_buffer)
      : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_P4;
        if(this->frame_buffer == nullptr) {
          this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < 8; i++) {
          palette[i].color = default_palette[i];
          palette[i].used = true;
        }
      }
      void set_pen(uint c) {
        color = c & 0xf;
      }
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override {
        // TODO look up closest palette colour, or just NOOP?
      }
      int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) override {
        i &= 0xf;
        palette[i].color = rgb_to_rgb565(r, g, b);
        palette[i].used = true;
        return i;
      }
      int reset_pen(uint8_t i) override {
        i &= 0xf;
        palette[i].color = default_palette[i];
        return i;
      }
      void set_pixel(const Point &p) override {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        uint8_t *f = &buf[(p.x / 2) + (p.y * bounds.w / 2)];

        uint8_t  o = (~p.x & 0b1) * 4; // bit offset within byte
        uint8_t  m = ~(0b1111 << o);   // bit mask for byte
        uint8_t  b = color << o;       // bit value shifted to position

        *f &= m; // clear bits
        *f |= b; // set value
      }
      void get_row_rgb565(void *result, uint offset, uint length) override {
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

  class PicoGraphics_PenP8 : public PicoGraphics {
    public:
      uint8_t color;
      PaletteEntry palette[256];
      PicoGraphics_PenP8(uint16_t width, uint16_t height, void *frame_buffer)
      : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_P8;
        if(this->frame_buffer == nullptr) {
          this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < 256; i++) {
          reset_pen(i);
        }
      }
      void set_pen(uint c) override {
        color = c;
      }
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override {
        // TODO look up closest palette colour, or just NOOP?
      }
      int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) override {
        i &= 0xff;
        palette[i].color = rgb_to_rgb565(r, g, b);
        palette[i].used = true;
        return i;
      }
      int reset_pen(uint8_t i) override {
        i &= 0xff;
        palette[i].color = 0;
        palette[i].used = false;
        return i;
      }
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override {
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
      void set_pixel(const Point &p) override {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
      }
      void get_row_rgb565(void *result, uint offset, uint length) override {
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

  class PicoGraphics_PenRGB332 : public PicoGraphics {
    public:
      RGB332 color;
      PaletteEntry palette[256];
      PicoGraphics_PenRGB332(uint16_t width, uint16_t height, void *frame_buffer)
      : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB332;
        if(this->frame_buffer == nullptr) {
          this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < 256; i++) {
          palette[i].color = rgb332_to_rgb565(i);
          palette[i].used = true;
        }
      }
      void set_pen(uint c) override {
        color = c;
      }
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override {
        color = rgb_to_rgb332(r, g, b);
      }
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override {
        return rgb_to_rgb332(r, g, b);
      }
      void set_pixel(const Point &p) override {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
      }
      void get_row_rgb565(void *result, uint offset, uint length) override {
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

  class PicoGraphics_PenRGB565 : public PicoGraphics {
    public:
      RGB565 color;
      PicoGraphics_PenRGB565(uint16_t width, uint16_t height, void *frame_buffer)
      : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB565;
        if(this->frame_buffer == nullptr) {
          this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
      }
      void set_pen(uint c) override {
        color = c;
      }
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override {
        color = rgb_to_rgb565(r, g, b);
      }
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override {
        return rgb_to_rgb565(r, g, b);
      }
      void set_pixel(const Point &p) override {
        uint16_t *buf = (uint16_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
      }
      static size_t buffer_size(uint w, uint h) {
        return w * h * sizeof(RGB565);
      }
  };

  class DisplayDriver {
    public:
      uint16_t width;
      uint16_t height;
      Rotation rotation;

      DisplayDriver(uint16_t width, uint16_t height, Rotation rotation)
       : width(width), height(height), rotation(rotation) {};

      virtual void update(PicoGraphics *display) {};
      virtual void set_backlight(uint8_t brightness) {};
  };

}
