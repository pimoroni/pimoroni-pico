#pragma once

#include <cstdint>

// a tiny little graphics library for our Pico products
// supports only 16-bit (565) RGB framebuffers
namespace pimoroni {

  class PicoGraphics {
  public:
    uint16_t width;
    uint16_t height;
    uint16_t *frame_buffer;
    uint16_t pen;

  public:
    PicoGraphics(uint16_t width, uint16_t height, uint16_t *frame_buffer)
      : frame_buffer(frame_buffer), width(width), height(height) {}

    void set_pen(uint8_t r, uint8_t g, uint8_t b);
    void set_pen(uint16_t p);
    uint16_t create_pen(uint8_t r, uint8_t g, uint8_t b);

    uint16_t* ptr(int32_t x, int32_t y);

    void pixel(int32_t x, int32_t y);
    void pixel_span(int32_t x, int32_t y, int32_t l);
    void rectangle(int32_t x, int32_t y, int32_t w, int32_t h);
    void circle(int32_t x, int32_t y, int32_t r);
    //void polygon(std::vector);
  };

}