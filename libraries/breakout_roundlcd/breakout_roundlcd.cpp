#include "breakout_roundlcd.hpp"

namespace pimoroni {

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  spi_inst_t *spi,
      uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso)  {
    __fb = buf;
  }

  void BreakoutRoundLCD::init() {
    // initialise the screen
    screen.init(true, true);
  }

  spi_inst_t* BreakoutRoundLCD::get_spi() const {
    return screen.get_spi();
  }

  int BreakoutRoundLCD::get_cs() const {
    return screen.get_cs();
  }

  int BreakoutRoundLCD::get_dc() const {
    return screen.get_dc();
  }

  int BreakoutRoundLCD::get_sck() const {
    return screen.get_sck();
  }

  int BreakoutRoundLCD::get_mosi() const {
    return screen.get_mosi();
  }

  int BreakoutRoundLCD::get_bl() const {
    return screen.get_bl();
  }

  void BreakoutRoundLCD::update() {
    screen.update();
  }

  void BreakoutRoundLCD::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}