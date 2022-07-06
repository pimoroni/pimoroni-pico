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
    color = c != 0 ? 1 : 0;
  }

  void PicoGraphics_Pen1Bit::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    color = r != 0 || g != 0 || b != 0 ? 1 : 0;
  }

  void PicoGraphics_Pen1Bit::set_pixel(const Point &p) {
    // pointer to byte in framebuffer that contains this pixel
    uint8_t *buf = (uint8_t *)frame_buffer;
    uint8_t *f = &buf[(p.x / 8) + (p.y * bounds.w / 8)];

    uint bo = 7 - (p.x & 0b111);

    // forceably clear the bit
    *f &= ~(1U << bo); 

    // set pixel
    *f |= (color << bo);
  }

  void PicoGraphics_Pen1Bit::set_pixel_span(const Point &p, uint l) {
    // pointer to byte in framebuffer that contains this pixel
    uint8_t *buf = (uint8_t *)frame_buffer;
    uint8_t *f = &buf[(p.x / 8) + (p.y * bounds.w / 8)];

    uint bo = 7 - (p.x & 0b111);

    // TODO: this could trivially be sped up by processing single bits only at 
    // the start and the end of the span and writing full bytes (8 pixels at
    // a time) in the middle portion of the span. would only be more efficient
    // for longer spans (probably around 20 pixels or more)
    while(l--) {
      // forceably clear the bit and then set to the correct value
      *f &= ~(1U << bo); 
      *f |= (color << bo);

      if(bo == 0) { // last bit of this byte?
        // move to next byte in framebuffer and reset the bit offset
        f++; bo = 8;
      }

      bo--;
    }
  }

  void PicoGraphics_Pen1Bit::set_pixel_dither(const Point &p, const uint8_t &c) {
    if(!bounds.contains(p)) return;

    uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];
    uint8_t _dc = c >> 4;
    color = _dc > _dmv ? 1 : 0;

    set_pixel(p);
  };

}