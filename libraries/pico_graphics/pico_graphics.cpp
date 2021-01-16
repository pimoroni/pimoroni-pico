#include "pico_graphics.hpp"

namespace pimoroni {

  void PicoGraphics::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    this->pen = create_pen(r, g, b);
  }

  void PicoGraphics::set_pen(uint16_t pen) {
    this->pen = pen;
  }

  uint16_t PicoGraphics::create_pen(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t p = ((r & 0b11111000) << 8) |
                 ((g & 0b11111100) << 3) |
                 ((b & 0b11111000) >> 3);

    // endian swap, this should be possible another way...
    return ((p & 0xff00) >> 8) | ((p & 0xff) << 8);
  }

  uint16_t* PicoGraphics::ptr(int32_t x, int32_t y) {
    return this->frame_buffer + x + y * this->width;
  }

  void PicoGraphics::pixel(int32_t x, int32_t y) {
    if(x < 0 || y < 0 || x >= this->width || y >= this->height) { return; }

    *ptr(x, y) = pen;
  }

  void PicoGraphics::pixel_span(int32_t x, int32_t y, int32_t l) {
    if(x + l < 0 || y < 0 || x >= this->width || y >= this->height) { return; }

    if(x < 0) {l += x; x = 0;}
    if(x + l >= this->width) {l = this->width - x;}

    uint16_t *p = ptr(x, y);
    while(l--) {
      *p++ = pen;
    }
  }

  void PicoGraphics::rectangle(int32_t x, int32_t y, int32_t width, int32_t height) {
    // clip and/or discard depending on rectangle visibility
    if(x >= this->width || y >= this->height) { return; }
    if(x < 0) { width  += x; x = 0; }
    if(y < 0) { height += y; y = 0; }
    if(width <= 0 || height <= 0) { return; }
    if(x + width >= this->width) { width = this->width - x; }
    if(y + height >= this->height) { height = this->height - y; }

    uint16_t *p = ptr(x, y);
    while(height--) {
      for(uint32_t i = 0; i < width; i++) {
        *p++ = this->pen;
      }
      p += this->width - width; // move to next scanline
    }
  }

  void PicoGraphics::circle(int32_t x, int32_t y, int32_t radius) {
    // circle in screen bounds?
    if(x + radius < 0 || y + radius < 0 || x - radius >= this->width || y - radius >= this->height) { return; }

    int ox = radius, oy = 0, err = -radius;
    while (ox >= oy)
    {
      int last_oy = oy;

      err += oy; oy++; err += oy;

      pixel_span(x - ox, y + last_oy, ox * 2 + 1);
      if (last_oy != 0) {
        pixel_span(x - ox, y - last_oy, ox * 2 + 1);
      }

      if (err >= 0) {
        if (ox != last_oy) {
          pixel_span(x - last_oy, y + ox, last_oy * 2 + 1);
          if (ox != 0) {
            pixel_span(x - last_oy, y - ox, last_oy * 2 + 1);
          }

          err -= ox; ox--; err -= ox;
        }
      }
    }
  }


}