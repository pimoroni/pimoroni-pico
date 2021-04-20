#include "breakout_roundlcd.hpp"

namespace pimoroni {

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint miso, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso, bl)  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  BG_SPI_SLOT slot) : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, slot) {
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