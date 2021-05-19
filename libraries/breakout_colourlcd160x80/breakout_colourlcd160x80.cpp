#include "breakout_colourlcd160x80.hpp"

namespace pimoroni {

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf) {
    __fb = buf;
  }

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf, spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint miso, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso, bl) {
    __fb = buf;
  }

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf, BG_SPI_SLOT slot)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, slot) {
    __fb = buf;
  }

  void BreakoutColourLCD160x80::init() {
    // initialise the screen
    screen.init();
  }

  spi_inst_t* BreakoutColourLCD160x80::get_spi() const {
    return screen.get_spi();
  }

  int BreakoutColourLCD160x80::get_cs() const {
    return screen.get_cs();
  }

  int BreakoutColourLCD160x80::get_dc() const {
    return screen.get_dc();
  }

  int BreakoutColourLCD160x80::get_sck() const {
    return screen.get_sck();
  }

  int BreakoutColourLCD160x80::get_mosi() const {
    return screen.get_mosi();
  }

  int BreakoutColourLCD160x80::get_bl() const {
    return screen.get_bl();
  }

  void BreakoutColourLCD160x80::update() {
    screen.update();
  }

  void BreakoutColourLCD160x80::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}