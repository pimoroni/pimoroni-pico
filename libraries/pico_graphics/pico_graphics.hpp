#pragma once

#include <string>
#include <array>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <vector>
#include <functional>

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
  typedef uint32_t RGB888;
  struct RGB {
    int16_t r, g, b;

    constexpr RGB() : r(0), g(0), b(0) {}
    constexpr RGB(RGB332 c) :
      r((c & 0b11100000) >> 0),
      g((c & 0b00011100) << 3),
      b((c & 0b00000011) << 6) {}
    constexpr RGB(RGB565 c) :
      r((__builtin_bswap16(c) & 0b1111100000000000) >> 8),
      g((__builtin_bswap16(c) & 0b0000011111100000) >> 3),
      b((__builtin_bswap16(c) & 0b0000000000011111) << 3) {}
    constexpr RGB(int16_t r, int16_t g, int16_t b) : r(r), g(g), b(b) {}

    constexpr RGB  operator+ (const RGB& c) const {return RGB(r + c.r, g + c.g, b + c.b);}
    constexpr RGB& operator+=(const RGB& c) {r += c.r; g += c.g; b += c.b; return *this;}
    constexpr RGB& operator-=(const RGB& c) {r -= c.r; g -= c.g; b -= c.b; return *this;}
    constexpr RGB  operator- (const RGB& c) const {return RGB(r - c.r, g - c.g, b - c.b);}

    // a rough approximation of how bright a colour is used to compare the
    // relative brightness of two colours
    int luminance() const {
      // weights based on https://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
      return r * 21 + g * 72 + b * 7;
    }

    // a relatively low cost approximation of how "different" two colours are
    // perceived which avoids expensive colour space conversions.
    // described in detail at https://www.compuphase.com/cmetric.htm
    int distance(const RGB& c) const {
      int rmean = (r + c.r) / 2;
      int rx = r - c.r;
      int gx = g - c.g;
      int bx = b - c.b;
      return abs((int)(
        (((512 + rmean) * rx * rx) >> 8) + 4 * gx * gx + (((767 - rmean) * bx * bx) >> 8)
      ));
    }

    int closest(const RGB *palette, size_t len) const {
      int d = INT_MAX, m = -1;
      for(size_t i = 0; i < len; i++) {
        int dc = distance(palette[i]);
        if(dc < d) {m = i; d = dc;}
      }
      return m;
    }

    constexpr RGB565 to_rgb565() {
      uint16_t p = ((r & 0b11111000) << 8) |
                   ((g & 0b11111100) << 3) |
                   ((b & 0b11111000) >> 3);

      return __builtin_bswap16(p);
    }

    constexpr RGB565 to_rgb332() {
      return (r & 0b11100000) | ((g & 0b11100000) >> 3) | ((b & 0b11000000) >> 6);
    }

    constexpr RGB888 to_rgb888() {
      return (r << 16) | (g << 8) | (b << 0);
    }
  };

  typedef int Pen;

  struct Rect;

  struct Point {
    int32_t x = 0, y = 0;

    Point() = default;
    Point(int32_t x, int32_t y) : x(x), y(y) {}

    inline Point& operator-= (const Point &a) { x -= a.x; y -= a.y; return *this; }
    inline Point& operator+= (const Point &a) { x += a.x; y += a.y; return *this; }
    inline Point& operator/= (const int32_t a) { x /= a;   y /= a;  return *this; }

    Point clamp(const Rect &r) const;
  };

  inline bool operator== (const Point &lhs, const Point &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
  inline bool operator!= (const Point &lhs, const Point &rhs) { return !(lhs == rhs); }
  inline Point operator-  (Point lhs, const Point &rhs) { lhs -= rhs; return lhs; }
  inline Point operator-  (const Point &rhs) { return Point(-rhs.x, -rhs.y); }
  inline Point operator+  (Point lhs, const Point &rhs) { lhs += rhs; return lhs; }
  inline Point operator/  (Point lhs, const int32_t a) { lhs /= a; return lhs; }

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

  static const RGB565 rgb332_to_rgb565_lut[256] = {
    0x0000, 0x0800, 0x1000, 0x1800, 0x0001, 0x0801, 0x1001, 0x1801, 0x0002, 0x0802, 0x1002, 0x1802, 0x0003, 0x0803, 0x1003, 0x1803,
    0x0004, 0x0804, 0x1004, 0x1804, 0x0005, 0x0805, 0x1005, 0x1805, 0x0006, 0x0806, 0x1006, 0x1806, 0x0007, 0x0807, 0x1007, 0x1807,
    0x0020, 0x0820, 0x1020, 0x1820, 0x0021, 0x0821, 0x1021, 0x1821, 0x0022, 0x0822, 0x1022, 0x1822, 0x0023, 0x0823, 0x1023, 0x1823,
    0x0024, 0x0824, 0x1024, 0x1824, 0x0025, 0x0825, 0x1025, 0x1825, 0x0026, 0x0826, 0x1026, 0x1826, 0x0027, 0x0827, 0x1027, 0x1827,
    0x0040, 0x0840, 0x1040, 0x1840, 0x0041, 0x0841, 0x1041, 0x1841, 0x0042, 0x0842, 0x1042, 0x1842, 0x0043, 0x0843, 0x1043, 0x1843,
    0x0044, 0x0844, 0x1044, 0x1844, 0x0045, 0x0845, 0x1045, 0x1845, 0x0046, 0x0846, 0x1046, 0x1846, 0x0047, 0x0847, 0x1047, 0x1847,
    0x0060, 0x0860, 0x1060, 0x1860, 0x0061, 0x0861, 0x1061, 0x1861, 0x0062, 0x0862, 0x1062, 0x1862, 0x0063, 0x0863, 0x1063, 0x1863,
    0x0064, 0x0864, 0x1064, 0x1864, 0x0065, 0x0865, 0x1065, 0x1865, 0x0066, 0x0866, 0x1066, 0x1866, 0x0067, 0x0867, 0x1067, 0x1867,
    0x0080, 0x0880, 0x1080, 0x1880, 0x0081, 0x0881, 0x1081, 0x1881, 0x0082, 0x0882, 0x1082, 0x1882, 0x0083, 0x0883, 0x1083, 0x1883,
    0x0084, 0x0884, 0x1084, 0x1884, 0x0085, 0x0885, 0x1085, 0x1885, 0x0086, 0x0886, 0x1086, 0x1886, 0x0087, 0x0887, 0x1087, 0x1887,
    0x00a0, 0x08a0, 0x10a0, 0x18a0, 0x00a1, 0x08a1, 0x10a1, 0x18a1, 0x00a2, 0x08a2, 0x10a2, 0x18a2, 0x00a3, 0x08a3, 0x10a3, 0x18a3,
    0x00a4, 0x08a4, 0x10a4, 0x18a4, 0x00a5, 0x08a5, 0x10a5, 0x18a5, 0x00a6, 0x08a6, 0x10a6, 0x18a6, 0x00a7, 0x08a7, 0x10a7, 0x18a7,
    0x00c0, 0x08c0, 0x10c0, 0x18c0, 0x00c1, 0x08c1, 0x10c1, 0x18c1, 0x00c2, 0x08c2, 0x10c2, 0x18c2, 0x00c3, 0x08c3, 0x10c3, 0x18c3,
    0x00c4, 0x08c4, 0x10c4, 0x18c4, 0x00c5, 0x08c5, 0x10c5, 0x18c5, 0x00c6, 0x08c6, 0x10c6, 0x18c6, 0x00c7, 0x08c7, 0x10c7, 0x18c7,
    0x00e0, 0x08e0, 0x10e0, 0x18e0, 0x00e1, 0x08e1, 0x10e1, 0x18e1, 0x00e2, 0x08e2, 0x10e2, 0x18e2, 0x00e3, 0x08e3, 0x10e3, 0x18e3,
    0x00e4, 0x08e4, 0x10e4, 0x18e4, 0x00e5, 0x08e5, 0x10e5, 0x18e5, 0x00e6, 0x08e6, 0x10e6, 0x18e6, 0x00e7, 0x08e7, 0x10e7, 0x18e7,
  };

  extern const uint8_t dither16_pattern[16];

  class PicoGraphics {
  public:
    enum PenType {
      PEN_1BIT,
      PEN_3BIT,
      PEN_P2,
      PEN_P4,
      PEN_P8,
      PEN_RGB332,
      PEN_RGB565,
      PEN_RGB888,
    };

    void *frame_buffer;

    PenType pen_type;
    Rect bounds;
    Rect clip;

    typedef std::function<void(void *data, size_t length)> conversion_callback_func;
    typedef std::function<RGB565()> next_pixel_func;
    //typedef std::function<void(int y)> scanline_interrupt_func;

    //scanline_interrupt_func scanline_interrupt = nullptr;

    const bitmap::font_t *bitmap_font;
    const hershey::font_t *hershey_font;

    static constexpr RGB332 rgb_to_rgb332(uint8_t r, uint8_t g, uint8_t b) {
      return RGB(r, g, b).to_rgb332();
    }

    static constexpr RGB565 rgb332_to_rgb565(RGB332 c) {
      uint16_t p = ((c & 0b11100000) << 8) |
                   ((c & 0b00011100) << 6) |
                   ((c & 0b00000011) << 3);
      return __builtin_bswap16(p);
    }

    static constexpr RGB565 rgb565_to_rgb332(RGB565 c) {
      c = __builtin_bswap16(c);
      return ((c & 0b1110000000000000) >> 8) |
             ((c & 0b0000011100000000) >> 6) |
             ((c & 0b0000000000011000) >> 3);
    }

    static constexpr RGB565 rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
      return RGB(r, g, b).to_rgb565();
    }

    static constexpr RGB rgb332_to_rgb(RGB332 c) {
      return RGB((RGB332)c);
    };

    static constexpr RGB rgb565_to_rgb(RGB565 c) {
      return RGB((RGB565)c);
    };

    PicoGraphics(uint16_t width, uint16_t height, void *frame_buffer)
    : frame_buffer(frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {
      set_font(&font6);
    };

    virtual void set_pen(uint c) = 0;
    virtual void set_pen(uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void set_pixel(const Point &p) = 0;
    virtual void set_pixel_span(const Point &p, uint l) = 0;

    virtual int create_pen(uint8_t r, uint8_t g, uint8_t b);
    virtual int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
    virtual int reset_pen(uint8_t i);
    virtual void set_pixel_dither(const Point &p, const RGB &c);
    virtual void set_pixel_dither(const Point &p, const RGB565 &c);
    virtual void set_pixel_dither(const Point &p, const uint8_t &c);
    virtual void frame_convert(PenType type, conversion_callback_func callback);
    virtual void sprite(void* data, const Point &sprite, const Point &dest, const int scale, const int transparent);

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
    void arc(const Point &center, const Point &start, int16_t degrees, float w = 1.0f);
    void triangle(Point p1, Point p2, Point p3);
    void line(Point p1, Point p2);

    float deg_to_rad(int16_t deg);
    int16_t rad_to_deg(float rad);

    float fast_sin(int16_t deg);
    float fast_cos(int16_t deg) {
      return this->fast_sin(deg + 90);
    };
  protected:
    void frame_convert_rgb565(conversion_callback_func callback, next_pixel_func get_next_pixel);
    std::vector<Point> arc_points(const Point &center, const Point &start, int16_t degrees);

    constexpr static const float sin_degs_lut[360] = {
        0, 0.017452406, 0.034899497, 0.052335956, 0.069756474, 0.087155743, 0.104528463, 0.121869343, 0.139173101,
        0.156434465, 0.173648178, 0.190808995, 0.207911691, 0.224951054, 0.241921896, 0.258819045, 0.275637356,
        0.292371705, 0.309016994, 0.325568154, 0.342020143, 0.35836795, 0.374606593, 0.390731128, 0.406736643,
        0.422618262, 0.438371147, 0.4539905, 0.469471563, 0.48480962, 0.5, 0.515038075, 0.529919264, 0.544639035,
        0.559192903, 0.573576436, 0.587785252, 0.601815023, 0.615661475, 0.629320391, 0.64278761, 0.656059029,
        0.669130606, 0.68199836, 0.69465837, 0.707106781, 0.7193398, 0.731353702, 0.743144825, 0.75470958,
        0.766044443, 0.777145961, 0.788010754, 0.79863551, 0.809016994, 0.819152044, 0.829037573, 0.838670568,
        0.848048096, 0.857167301, 0.866025404, 0.874619707, 0.882947593, 0.891006524, 0.898794046, 0.906307787,
        0.913545458, 0.920504853, 0.927183855, 0.933580426, 0.939692621, 0.945518576, 0.951056516, 0.956304756,
        0.961261696, 0.965925826, 0.970295726, 0.974370065, 0.978147601, 0.981627183, 0.984807753, 0.987688341,
        0.990268069, 0.992546152, 0.994521895, 0.996194698, 0.99756405, 0.998629535, 0.999390827, 0.999847695, 1,
        0.999847695, 0.999390827, 0.998629535, 0.99756405, 0.996194698, 0.994521895, 0.992546152, 0.990268069,
        0.987688341, 0.984807753, 0.981627183, 0.978147601, 0.974370065, 0.970295726, 0.965925826, 0.961261696,
        0.956304756, 0.951056516, 0.945518576, 0.939692621, 0.933580426, 0.927183855, 0.920504853, 0.913545458,
        0.906307787, 0.898794046, 0.891006524, 0.882947593, 0.874619707, 0.866025404, 0.857167301, 0.848048096,
        0.838670568, 0.829037573, 0.819152044, 0.809016994, 0.79863551, 0.788010754, 0.777145961, 0.766044443,
        0.75470958, 0.743144825, 0.731353702, 0.7193398, 0.707106781, 0.69465837, 0.68199836, 0.669130606,
        0.656059029, 0.64278761, 0.629320391, 0.615661475, 0.601815023, 0.587785252, 0.573576436, 0.559192903,
        0.544639035, 0.529919264, 0.515038075, 0.5, 0.48480962, 0.469471563, 0.4539905, 0.438371147, 0.422618262,
        0.406736643, 0.390731128, 0.374606593, 0.35836795, 0.342020143, 0.325568154, 0.309016994, 0.292371705,
        0.275637356, 0.258819045, 0.241921896, 0.224951054, 0.207911691, 0.190808995, 0.173648178, 0.156434465,
        0.139173101, 0.121869343, 0.104528463, 0.087155743, 0.069756474, 0.052335956, 0.034899497, 0.017452406,
        1.22515E-16, -0.017452406, -0.034899497, -0.052335956, -0.069756474, -0.087155743, -0.104528463,
        -0.121869343, -0.139173101, -0.156434465, -0.173648178, -0.190808995, -0.207911691, -0.224951054,
        -0.241921896, -0.258819045, -0.275637356, -0.292371705, -0.309016994, -0.325568154, -0.342020143,
        -0.35836795, -0.374606593, -0.390731128, -0.406736643, -0.422618262, -0.438371147, -0.4539905,
        -0.469471563, -0.48480962, -0.5, -0.515038075, -0.529919264, -0.544639035, -0.559192903, -0.573576436,
        -0.587785252, -0.601815023, -0.615661475, -0.629320391, -0.64278761, -0.656059029, -0.669130606,
        -0.68199836, -0.69465837, -0.707106781, -0.7193398, -0.731353702, -0.743144825, -0.75470958, -0.766044443,
        -0.777145961, -0.788010754, -0.79863551, -0.809016994, -0.819152044, -0.829037573, -0.838670568,
        -0.848048096, -0.857167301, -0.866025404, -0.874619707, -0.882947593, -0.891006524, -0.898794046,
        -0.906307787, -0.913545458, -0.920504853, -0.927183855, -0.933580426, -0.939692621, -0.945518576,
        -0.951056516, -0.956304756, -0.961261696, -0.965925826, -0.970295726, -0.974370065, -0.978147601,
        -0.981627183, -0.984807753, -0.987688341, -0.990268069, -0.992546152, -0.994521895, -0.996194698,
        -0.99756405, -0.998629535, -0.999390827, -0.999847695, -1, -0.999847695, -0.999390827, -0.998629535,
        -0.99756405, -0.996194698, -0.994521895, -0.992546152, -0.990268069, -0.987688341, -0.984807753,
        -0.981627183, -0.978147601, -0.974370065, -0.970295726, -0.965925826, -0.961261696, -0.956304756,
        -0.951056516, -0.945518576, -0.939692621, -0.933580426, -0.927183855, -0.920504853, -0.913545458,
        -0.906307787, -0.898794046, -0.891006524, -0.882947593, -0.874619707, -0.866025404, -0.857167301,
        -0.848048096, -0.838670568, -0.829037573, -0.819152044, -0.809016994, -0.79863551, -0.788010754,
        -0.777145961, -0.766044443, -0.75470958, -0.743144825, -0.731353702, -0.7193398, -0.707106781,
        -0.69465837, -0.68199836, -0.669130606, -0.656059029, -0.64278761, -0.629320391, -0.615661475,
        -0.601815023, -0.587785252, -0.573576436, -0.559192903, -0.544639035, -0.529919264, -0.515038075, -0.5,
        -0.48480962, -0.469471563, -0.4539905, -0.438371147, -0.422618262, -0.406736643, -0.390731128,
        -0.374606593, -0.35836795, -0.342020143, -0.325568154, -0.309016994, -0.292371705, -0.275637356,
        -0.258819045, -0.241921896, -0.224951054, -0.207911691, -0.190808995, -0.173648178, -0.156434465,
        -0.139173101, -0.121869343, -0.104528463, -0.087155743, -0.069756474, -0.052335956, -0.034899497,
        -0.017452406
      };
  };

  class PicoGraphics_Pen1Bit : public PicoGraphics {
    public:
      uint8_t color;
    
      PicoGraphics_Pen1Bit(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;

      static size_t buffer_size(uint w, uint h) {
          return w * h / 8;
      }
  };

  class PicoGraphics_Pen1BitY : public PicoGraphics {
    public:
      uint8_t color;
    
      PicoGraphics_Pen1BitY(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;

      static size_t buffer_size(uint w, uint h) {
          return w * h / 8;
      }
  };

  class PicoGraphics_Pen3Bit : public PicoGraphics {
    public:
      static const uint16_t palette_size = 8;
      uint8_t color;
      RGB palette[8] = {
        /*
        {0x2b, 0x2a, 0x37},
        {0xdc, 0xcb, 0xba},
        {0x35, 0x56, 0x33},
        {0x33, 0x31, 0x47},
        {0x9c, 0x3b, 0x2e},
        {0xd3, 0xa9, 0x34},
        {0xab, 0x58, 0x37},
        {0xb2, 0x8e, 0x67}
        */
        {  0,   0,   0}, // black
        {255, 255, 255}, // white
        {  0, 255,   0}, // green
        {  0,   0, 255}, // blue
        {255,   0,   0}, // red
        {255, 255,   0}, // yellow
        {255, 128,   0}, // orange
        {220, 180, 200}  // clean / taupe?!
      };

      std::array<std::array<uint8_t, 16>, 512> candidate_cache;
      bool cache_built = false;
      std::array<uint8_t, 16> candidates;

      PicoGraphics_Pen3Bit(uint16_t width, uint16_t height, void *frame_buffer);

      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      void get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates);
      void set_pixel_dither(const Point &p, const RGB &c) override;

      void frame_convert(PenType type, conversion_callback_func callback) override;
      static size_t buffer_size(uint w, uint h) {
          return (w * h / 8) * 3;
      }
  };

  class PicoGraphics_PenP4 : public PicoGraphics {
    public:
      static const uint16_t palette_size = 16;
      uint8_t color;
      RGB palette[palette_size];
      bool used[palette_size];

      std::array<std::array<uint8_t, 16>, 512> candidate_cache;
      bool cache_built = false;
      std::array<uint8_t, 16> candidates;

      PicoGraphics_PenP4(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) override;
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int reset_pen(uint8_t i) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      void get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates);
      void set_pixel_dither(const Point &p, const RGB &c) override;

      void frame_convert(PenType type, conversion_callback_func callback) override;
      static size_t buffer_size(uint w, uint h) {
          return w * h / 2;
      }
  };

  class PicoGraphics_PenP8 : public PicoGraphics {
    public:
      static const uint16_t palette_size = 256;
      uint8_t color;
      RGB palette[palette_size];
      bool used[palette_size];
    
      std::array<std::array<uint8_t, 16>, 512> candidate_cache;
      bool cache_built = false;
      std::array<uint8_t, 16> candidates;

      PicoGraphics_PenP8(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) override;
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int reset_pen(uint8_t i) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      void get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates);
      void set_pixel_dither(const Point &p, const RGB &c) override;

      void frame_convert(PenType type, conversion_callback_func callback) override;
      static size_t buffer_size(uint w, uint h) {
        return w * h;
      }
  };

  class PicoGraphics_PenRGB332 : public PicoGraphics {
    public:
      RGB332 color;
      PicoGraphics_PenRGB332(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override;

      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      void set_pixel_dither(const Point &p, const RGB &c) override;
      void set_pixel_dither(const Point &p, const RGB565 &c) override;

      void sprite(void* data, const Point &sprite, const Point &dest, const int scale, const int transparent) override;

      void frame_convert(PenType type, conversion_callback_func callback) override;
      static size_t buffer_size(uint w, uint h) {
        return w * h;
      }
  };

  class PicoGraphics_PenRGB565 : public PicoGraphics {
    public:
      RGB src_color;
      RGB565 color;
      PicoGraphics_PenRGB565(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override;
      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      static size_t buffer_size(uint w, uint h) {
        return w * h * sizeof(RGB565);
      }
  };

  
  class PicoGraphics_PenRGB888 : public PicoGraphics {
    public:
      RGB src_color;
      RGB888 color;
      PicoGraphics_PenRGB888(uint16_t width, uint16_t height, void *frame_buffer);
      void set_pen(uint c) override;
      void set_pen(uint8_t r, uint8_t g, uint8_t b) override;
      int create_pen(uint8_t r, uint8_t g, uint8_t b) override;
      void set_pixel(const Point &p) override;
      void set_pixel_span(const Point &p, uint l) override;
      static size_t buffer_size(uint w, uint h) {
        return w * h * sizeof(uint32_t);
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
      virtual void partial_update(PicoGraphics *display, Rect region) {};
      virtual bool set_update_speed(int update_speed) {return false;};
      virtual void set_backlight(uint8_t brightness) {};
      virtual bool is_busy() {return false;};
      virtual void power_off() {};
      virtual void cleanup() {};
  };

}
