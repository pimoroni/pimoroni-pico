#pragma once

#include "../../drivers/st7789/st7789.hpp"
#include "../../libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class BreakoutColourLCD160x80 : public PicoGraphics {
  public:
    static const int WIDTH = 168;
    static const int HEIGHT = 80;

    uint16_t *__fb;
  private:
    ST7789 screen;

  public:
    BreakoutColourLCD160x80(uint16_t *buf);

    void init();
    void update();
    void set_backlight(uint8_t brightness);
  };

}