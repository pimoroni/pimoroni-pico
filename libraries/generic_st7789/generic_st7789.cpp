#include <math.h>
#include <string.h>

#include "generic_st7789.hpp"


namespace pimoroni {
  void ST7789Generic::update() {
    st7789.update(palette);
  }

  void ST7789Generic::set_backlight(uint8_t brightness) {
    st7789.set_backlight(brightness);
  }
}
