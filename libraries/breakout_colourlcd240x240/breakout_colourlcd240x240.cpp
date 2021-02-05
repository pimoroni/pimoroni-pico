#include "breakout_colourlcd240x240.hpp"

namespace pimoroni {

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  void BreakoutColourLCD240x240::init() {
    // initialise the screen
    screen.init();
  }

  void BreakoutColourLCD240x240::update() {
    screen.update();
  }

  void BreakoutColourLCD240x240::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}