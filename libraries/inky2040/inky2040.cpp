#include <string.h>
#include <math.h>

#include "hardware/pwm.h"
#include "hardware/watchdog.h"

#include "inky2040.hpp"

namespace pimoroni {

  void Inky2040::init() {
    uc8159.init();
  }

  void Inky2040::clear() {
    for(int32_t y = 0; y < HEIGHT; y++) {
      for(int32_t x = 0; x < WIDTH; x++) {
        pixel(x, y);
      }
    }
  }

  void Inky2040::halt() {
  }

  void Inky2040::pixel(int32_t x, int32_t y) {
    if(_thickness == 1) {
      uc8159.pixel(x, y, _pen);
    }else{
      uint8_t ht = _thickness / 2;
      for(int sy = 0; sy < _thickness; sy++) {
        for(int sx = 0; sx < _thickness; sx++) {
          uc8159.pixel(x + sx - ht, y + sy - ht, _pen);
        }
      }
    }
  }

  // Display a portion of an image (icon sheet) at dx, dy
  void Inky2040::icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy) {
    image(data, sheet_width, icon_size * index, 0, icon_size, icon_size, dx, dy);
  }

  // Display an image that fills the screen (286*128)
  void Inky2040::image(const uint8_t* data) {
    image(data, WIDTH, 0, 0, WIDTH, HEIGHT, 0, 0);
  }

  // Display an image smaller than the screen (sw*sh) at dx, dy
  void Inky2040::image(const uint8_t *data, int w, int h, int x, int y) {
    image(data, w, 0, 0, w, h, x, y);
  }

  void Inky2040::image(const uint8_t *data, int stride, int sx, int sy, int dw, int dh, int dx, int dy) {
    for(auto y = 0; y < dh; y++) {
      for(auto x = 0; x < dw; x++) {

        uint32_t o = ((y + sy) * (stride / 2)) + ((x + sx) / 2);
        uint8_t  d = ((x + sx) & 0b1) ? data[o] >> 4 : data[o] & 0xf;

        // draw the pixel
        uc8159.pixel(dx + x, dy + y, d);
      }
    }
  }

  void Inky2040::rectangle(int32_t x, int32_t y, int32_t w, int32_t h) {
    for(int cy = y; cy < y + h; cy++) {
      for(int cx = x; cx < x + w; cx++) {
        pixel(cx, cy);
      }
    }
  }

  void Inky2040::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    int32_t	x = x1, y = y1, dx, dy, incx, incy, balance;

    if(x2 >= x1) {dx = x2 - x1; incx = 1;} else {dx = x1 - x2; incx = -1;}
    if(y2 >= y1) {dy = y2 - y1; incy = 1;} else {dy = y1 - y2; incy = -1;}

    if(dx >= dy) {
      dy <<= 1; balance = dy - dx; dx <<= 1;
      while(x != x2) {
        pixel(x, y);
        if (balance >= 0) {y += incy; balance -= dx;}
        balance += dy; x += incx;
      }
    } else {
      dx <<= 1; balance = dx - dy; dy <<= 1;
      while(y != y2) {
        pixel(x, y);
        if(balance >= 0) {x += incx; balance -= dy;}
        balance += dx; y += incy;
      }
    }
  }

  bool Inky2040::is_busy() {
    return uc8159.is_busy();
  }

  void Inky2040::power_off() {
    uc8159.power_off();
  }

  void Inky2040::update(bool blocking) {
    uc8159.update(blocking);
  }

  const hershey::font_glyph_t* Inky2040::glyph_data(unsigned char c) {
    return hershey::glyph_data(_font, c);
  }

  int32_t Inky2040::glyph(unsigned char c, int32_t x, int32_t y, float s, float a) {
    if (_bitmap_font) {
      bitmap::character(_bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        for(auto px = 0; px < w; px++) {
          for(auto py = 0; py < h; py++) {
            pixel(x + px, y + py);
          }
        }
      }, c, x, y, int(s));
      return 0;
    } else {
      return hershey::glyph(_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
        line(x1, y1, x2, y2);
      }, c, x, y, s, a);
    }
  }

  void Inky2040::text(std::string message, int32_t x, int32_t y, float s, float a) {
    if (_bitmap_font) {
      bitmap::text(_bitmap_font, [this](int32_t x, int32_t y, int32_t w, int32_t h) {
        for(auto px = 0; px < w; px++) {
          for(auto py = 0; py < h; py++) {
            pixel(x + px, y + py);
          }
        }
      }, message, x, y, INT32_MAX, int(s)); // We basically want to disable wrapping, since it wont really work here, but Pico Graphics needs it
    } else {
      hershey::text(_font, [this](int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
        line(x1, y1, x2, y2);
      }, message, x, y, s, a);
    }
  }

  int32_t Inky2040::measure_text(std::string message, float s) {
    if (_bitmap_font) return bitmap::measure_text(_bitmap_font, message, s);
    return hershey::measure_text(_font, message, s);
  }

  int32_t Inky2040::measure_glyph(unsigned char c, float s) {
    if (_bitmap_font) return bitmap::measure_character(_bitmap_font, c, s);
    return hershey::measure_glyph(_font, c, s);
  }

  void Inky2040::font(std::string name) {
    if (name == "bitmap6") {
      _bitmap_font = &font6;
      _font = nullptr;
    } else if (name == "bitmap8") {
      _bitmap_font = &font8;
      _font = nullptr;
    } else {
      // check that font exists and assign it
      if(hershey::fonts.find(name) != hershey::fonts.end()) {
        _bitmap_font = nullptr;
        _font = hershey::fonts[name];
      }
    }
  }

  void Inky2040::pen(uint8_t pen) {
    _pen = pen;
  }

  void Inky2040::thickness(uint8_t thickness) {
    _thickness = thickness;
  }
}
