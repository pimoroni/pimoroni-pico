#include <math.h>
#include <string.h>

#include "picographics_st7789.hpp"


namespace pimoroni {
  void PicoGraphicsST7789::update() {
    st7789.update(palette);
  }

  void PicoGraphicsST7789::set_backlight(uint8_t brightness) {
    st7789.set_backlight(brightness);
  }
}
