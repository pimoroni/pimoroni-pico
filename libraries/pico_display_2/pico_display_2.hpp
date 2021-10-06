#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class PicoDisplay2 : public PicoGraphics {
  public:
    static const int WIDTH = 320;
    static const int HEIGHT = 240;
    static const int PORTRAIT_WIDTH  = 240;
    static const int PORTRAIT_HEIGHT = 320;
     static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;

    uint16_t *__fb;
  private:
    ST7789 screen;

  public:
    PicoDisplay2(uint16_t *buf);
    PicoDisplay2(uint16_t *buf, int width, int height);

    void init();
    void update();
    void set_backlight(uint8_t brightness);
    void set_led(uint8_t r, uint8_t g, uint8_t b);
    bool is_pressed(uint8_t button);
    void flip();
  };

}
