#pragma once

#include "drivers/ili9341/ili9341.hpp"
#include "drivers/xpt2046/xpt2046.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {
  // class for a simple calibrate screen
  class Calibrate_Screen {
    public:
      static void run_calibration(ILI9341* ili9341, XPT2046* xpt2046, PicoGraphics* graphics);

    private:
      static void draw_cross(PicoGraphics* graphics, uint x, uint y, uint size);
      static void draw_calibrate_background(PicoGraphics* graphics, const char* str);
      static TouchPoint get_raw_touch(XPT2046* xpt2046);
  };
}