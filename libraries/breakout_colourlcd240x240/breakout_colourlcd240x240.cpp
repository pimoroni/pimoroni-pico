#include "breakout_colourlcd240x240.hpp"

namespace pimoroni {

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, false, buf,
        PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, SPI_BG_FRONT_PWM)  {
    __fb = buf;
  }

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint bl)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, false, buf, spi, cs, dc, sck, mosi, bl)  {
    __fb = buf;
  }

  BreakoutColourLCD240x240::BreakoutColourLCD240x240(uint16_t *buf,  BG_SPI_SLOT slot)
    : PicoGraphics(WIDTH, HEIGHT, buf), screen(WIDTH, HEIGHT, false, buf,
        PIMORONI_SPI_DEFAULT_INSTANCE, screen.get_slot_cs(slot), SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, screen.get_slot_bl(slot)) {
    __fb = buf;
  }

  void BreakoutColourLCD240x240::init() {
  }

  spi_inst_t* BreakoutColourLCD240x240::get_spi() const {
    return screen.get_spi();
  }

  int BreakoutColourLCD240x240::get_cs() const {
    return screen.get_cs();
  }

  int BreakoutColourLCD240x240::get_dc() const {
    return screen.get_dc();
  }

  int BreakoutColourLCD240x240::get_sck() const {
    return screen.get_sck();
  }

  int BreakoutColourLCD240x240::get_mosi() const {
    return screen.get_mosi();
  }

  int BreakoutColourLCD240x240::get_bl() const {
    return screen.get_bl();
  }

  void BreakoutColourLCD240x240::update() {
    screen.update();
  }

  void BreakoutColourLCD240x240::set_backlight(uint8_t brightness) {
    screen.set_backlight(brightness);
  }

}