#include "breakout_colourlcd160x80.hpp"

namespace pimoroni {

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  void BreakoutColourLCD160x80::init() {
    // initialise the screen
    screen.init();
  }

  void BreakoutColourLCD160x80::update() {
    screen.update();
  }

  void BreakoutColourLCD160x80::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}