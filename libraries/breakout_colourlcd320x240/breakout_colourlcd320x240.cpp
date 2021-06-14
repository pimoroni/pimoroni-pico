#include "breakout_colourlcd320x240.hpp"

namespace pimoroni {

  BreakoutColourLCD320x240::BreakoutColourLCD320x240(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf)  {
    __fb = buf;
  }

  BreakoutColourLCD320x240::BreakoutColourLCD320x240(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint miso, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, spi, cs, dc, sck, mosi, miso, bl)  {
    __fb = buf;
  }

  BreakoutColourLCD320x240::BreakoutColourLCD320x240(uint16_t *buf,  BG_SPI_SLOT slot)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, buf, slot) {
    __fb = buf;
  }

  void BreakoutColourLCD320x240::init() {
    // initialise the screen
    screen.init(true, false, 74 * 1000 * 1000);
  }

  spi_inst_t* BreakoutColourLCD320x240::get_spi() const {
    return screen.get_spi();
  }

  int BreakoutColourLCD320x240::get_cs() const {
    return screen.get_cs();
  }

  int BreakoutColourLCD320x240::get_dc() const {
    return screen.get_dc();
  }

  int BreakoutColourLCD320x240::get_sck() const {
    return screen.get_sck();
  }

  int BreakoutColourLCD320x240::get_mosi() const {
    return screen.get_mosi();
  }

  int BreakoutColourLCD320x240::get_bl() const {
    return screen.get_bl();
  }

  void BreakoutColourLCD320x240::update() {
    screen.update();
  }

  void BreakoutColourLCD320x240::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}