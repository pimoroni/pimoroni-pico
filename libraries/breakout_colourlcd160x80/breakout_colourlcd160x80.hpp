#pragma once

#include "../../drivers/st7735/st7735.hpp"
#include "../../libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class BreakoutColourLCD160x80 : public PicoGraphics {
  public:
    static const int WIDTH = 80;
    static const int HEIGHT = 160;

    uint16_t *__fb;
  private:
    ST7735 screen;

  public:
    BreakoutColourLCD160x80(uint16_t *buf);

    void init();
    void update();
    void set_backlight(uint8_t brightness);
  };

}