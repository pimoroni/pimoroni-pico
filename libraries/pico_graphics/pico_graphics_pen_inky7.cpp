#include "pico_graphics.hpp"

namespace pimoroni {
  PicoGraphics_PenInky7::PicoGraphics_PenInky7(uint16_t width, uint16_t height, IDirectDisplayDriver<uint8_t> &direct_display_driver)
  : PicoGraphics(width, height, nullptr),
    driver(direct_display_driver) {
      this->pen_type = PEN_INKY7;
  }
  void PicoGraphics_PenInky7::set_pen(uint c) {
    color = c & 0x7;
  }
  void PicoGraphics_PenInky7::set_pen(uint8_t r, uint8_t g, uint8_t b) {
  }
  int PicoGraphics_PenInky7::create_pen(uint8_t r, uint8_t g, uint8_t b) {
    return 0;
  }
  void PicoGraphics_PenInky7::set_pixel(const Point &p) {
    driver.write_pixel(p, color);
  }
  void PicoGraphics_PenInky7::set_pixel_span(const Point &p, uint l) {
    driver.write_pixel_span(p, l, color);
  }
  void PicoGraphics_PenInky7::frame_convert(PenType type, conversion_callback_func callback) {
    if(type == PEN_INKY7) {
      uint byte_count = bounds.w/2;
      uint8_t buffer[bounds.w];

      for(int32_t r = 0; r < bounds.h; r++)
      {
        driver.read_pixel_span(Point(0, r), bounds.w, buffer);
        // for(int y=0; y < 800; y++)
        //   buffer[y] = rand() % 7;

        // convert byte storage to nibble storage
        uint8_t *pDst = buffer;
        uint8_t *pSrc = buffer;
                
        for(uint c = 0; c < byte_count; c++)
        {
          uint8_t nibble = ((*pSrc++) << 4);
          nibble |= ((*pSrc++) & 0xf);
          *pDst++ = nibble;
        }

        callback(buffer, byte_count);
      }
    }
  }
}