#include "pico_graphics.hpp"

namespace pimoroni {

  PicoGraphics_Pen1Bit::PicoGraphics_Pen1Bit(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
    this->pen_type = PEN_1BIT;
    if(this->frame_buffer == nullptr) {
      this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
    }
  }

  void PicoGraphics_Pen1Bit::set_pen(uint c) {
    color = c;
  }

  void PicoGraphics_Pen1Bit::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    color = std::max(r, std::max(g, b)) >> 4;
  }

  void PicoGraphics_Pen1Bit::set_pixel(const Point &p) {
    // pointer to byte in framebuffer that contains this pixel
    uint8_t *buf = (uint8_t *)frame_buffer;
    uint8_t *f = &buf[(p.x / 8) + (p.y * bounds.w / 8)];

    uint bo = 7 - (p.x & 0b111);

    uint8_t _dc = 0;

    if(color == 0) {
      _dc = 0;
    } else if (color == 15) {
      _dc = 1;
    } else {
      uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];
      _dc = color > _dmv ? 1 : 0;
    }

    // forceably clear the bit
    *f &= ~(1U << bo); 

    // set pixel
    *f |= (_dc << bo);
  }

  void PicoGraphics_Pen1Bit::set_pixel_span(const Point &p, uint l) {
    Point lp = p;
    if(p.x + (int)l >= bounds.w) {
      l = bounds.w - p.x;
    }
    while(l--) {
        set_pixel(lp);
        lp.x++;
    }
  }

}