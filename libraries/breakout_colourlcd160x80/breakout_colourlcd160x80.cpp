#include "breakout_colourlcd160x80.hpp"

namespace pimoroni {

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  BreakoutColourLCD160x80::BreakoutColourLCD160x80(uint16_t *buf,  spi_inst_t *spi,
      uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso)  {
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

  int BreakoutColourLCD160x80::get_miso() const {
    return screen.get_miso();
  }

  void BreakoutColourLCD160x80::update() {
    screen.update();
  }

  void BreakoutColourLCD160x80::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}