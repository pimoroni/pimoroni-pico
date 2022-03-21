#include <math.h>
#include <string.h>

#include "generic_st7789.hpp"


namespace pimoroni {

  ST7789Generic::ST7789Generic(int width, int height, BG_SPI_SLOT slot, uint16_t *frame_buffer)
    : PicoGraphics(width, height, frame_buffer), st7789(width, height, frame_buffer, slot) {
      this->frame_buffer = st7789.frame_buffer;
      this->st7789.init(true, false);
  }

  ST7789Generic::ST7789Generic(uint16_t width, uint16_t height, spi_inst_t *spi,
           uint cs, uint dc, uint sck, uint mosi, uint miso, uint bl,
            uint16_t *frame_buffer) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, frame_buffer, spi, cs, dc, sck, mosi, miso, bl) {
      this->frame_buffer = st7789.frame_buffer;
      this->st7789.init(true, false);
  }

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
    st7789.update();
  }

  void ST7789Generic::flip() {
    st7789.flip();
  }

  void ST7789Generic::set_backlight(uint8_t brightness) {
    st7789.set_backlight(brightness);
  }
}
