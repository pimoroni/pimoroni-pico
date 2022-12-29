#include "pico_graphics.hpp"

namespace pimoroni {
  PicoGraphics_PenRGB565_direct::PicoGraphics_PenRGB565_direct(uint16_t width, uint16_t height, IDirectDisplayDriver<uint16_t> &direct_display_driver)
  : PicoGraphics(width, height, nullptr),
    driver(direct_display_driver) {
      this->pen_type = PEN_RGB565;
  }
  void PicoGraphics_PenRGB565_direct::set_pen(uint c) {
    color = c;
  }
  void PicoGraphics_PenRGB565_direct::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    src_color = {r, g, b};
    color = src_color.to_rgb565();
  }
  int PicoGraphics_PenRGB565_direct::create_pen(uint8_t r, uint8_t g, uint8_t b) {
    return RGB(r, g, b).to_rgb565();
  }
  void PicoGraphics_PenRGB565_direct::set_pixel(const Point &p) {
    driver.write_pixel(p, color);
  }
  void PicoGraphics_PenRGB565_direct::set_pixel_span(const Point &p, uint l) {
    driver.write_pixel_span(p, l, color);
  }
}