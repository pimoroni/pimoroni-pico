#include "breakout_roundlcd.hpp"

namespace pimoroni {

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, true, buf,
        PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, SPI_BG_FRONT_PWM)  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, true, buf, spi, cs, dc, sck, mosi, bl)  {
    __fb = buf;
  }

  BreakoutRoundLCD::BreakoutRoundLCD(uint16_t *buf,  BG_SPI_SLOT slot)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, true, buf,
        PIMORONI_SPI_DEFAULT_INSTANCE, screen.get_slot_cs(slot), SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, screen.get_slot_bl(slot)) {
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