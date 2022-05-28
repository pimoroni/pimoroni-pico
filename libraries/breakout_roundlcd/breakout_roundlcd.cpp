#include "breakout_roundlcd.hpp"

namespace pimoroni {

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf),
      screen(WIDTH, HEIGHT, ROTATE_0, true, buf, get_spi_pins(BG_SPI_FRONT)) {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf),
      screen(WIDTH, HEIGHT, ROTATE_0, true, buf, SPIPins{spi, cs sck, mosi, PIN_UNUSED, dc, bl})  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  BG_SPI_SLOT slot)
    : PicoGraphics(WIDTH, HEIGHT, buf),
      screen(WIDTH, HEIGHT, ROTATE_0, true, buf, get_spi_pins(slot)) { {
    __fb = buf;
  }

  void BreakoutRoundLCD::init() {
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