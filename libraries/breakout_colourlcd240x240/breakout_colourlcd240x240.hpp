#pragma once

#include "../../drivers/st7789/st7789.hpp"
#include "../../libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class BreakoutColourLCD240x240 : public PicoGraphics {
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 240;

    uint16_t *__fb;
  private:
    ST7789 screen;

  public:
    BreakoutColourLCD240x240(uint16_t *buf);

    void init();
    void update();
    void set_backlight(uint8_t brightness);
  };

}