#include <math.h>
#include <string.h>

#include "generic_st7789.hpp"


namespace pimoroni {
  spi_inst_t* ST7789Generic::get_spi() const {
    return st7789.get_spi();
  }

  int ST7789Generic::get_cs() const {
    return st7789.get_cs();
  }

  int ST7789Generic::get_dc() const {
    return st7789.get_dc();
  }

  int ST7789Generic::get_sck() const {
    return st7789.get_sck();
  }

  int ST7789Generic::get_mosi() const {
    return st7789.get_mosi();
  }

  int ST7789Generic::get_bl() const {
    return st7789.get_bl();
  }

  void ST7789Generic::update() {
    st7789.update(palette);
  }

  void ST7789Generic::flip() {
    st7789.configure_display(true);
  }

  void ST7789Generic::set_backlight(uint8_t brightness) {
    st7789.set_backlight(brightness);
  }

  void ST7789Generic::configure_display(bool rotate180) {
    st7789.configure_display(rotate180);
  }
}
