#include "pico_graphics.hpp"

namespace pimoroni {

  PicoGraphics_Pen1BitY::PicoGraphics_Pen1BitY(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
    this->pen_type = PEN_1BIT;
    if(this->frame_buffer == nullptr) {
      this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
    }
  }

  void PicoGraphics_Pen1BitY::set_pen(uint c) {
    color = c != 0 ? 1 : 0;
  }

  void PicoGraphics_Pen1BitY::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    color = r != 0 || g != 0 || b != 0 ? 1 : 0;
  }

  void PicoGraphics_Pen1BitY::set_pixel(const Point &p) {
    // pointer to byte in framebuffer that contains this pixel
    uint8_t *buf = (uint8_t *)frame_buffer;
    uint8_t *f = &buf[(p.y / 8) + (p.x * bounds.h / 8)];

    uint bo = 7 - (p.y & 0b111);

    // forceably clear the bit
    *f &= ~(1U << bo); 

    // set pixel
    *f |= (color << bo);
  }

  void PicoGraphics_Pen1BitY::set_pixel_span(const Point &p, uint l) {
    Point po(p);
    while(l--) {
      set_pixel(po);
      po.x++;
    }
  }

  void PicoGraphics_Pen1BitY::set_pixel_dither(const Point &p, const uint8_t &c) {
    if(!bounds.contains(p)) return;

    uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];
    uint8_t _dc = c >> 4;
    color = _dc > _dmv ? 1 : 0;

    set_pixel(p);
  };

}