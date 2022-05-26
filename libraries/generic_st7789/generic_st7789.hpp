#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class ST7789Generic : public PicoGraphics {
  private:
    ST7789 st7789;

  public:
    ST7789Generic(uint16_t width, uint16_t height, bool round=false, void *frame_buffer=nullptr) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, round, frame_buffer, PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, SPI_BG_FRONT_PWM) {
            this->frame_buffer = (Pen *)st7789.frame_buffer;
            this->st7789.init();
           };

    ST7789Generic(uint16_t width, uint16_t height, bool round, void *frame_buffer, BG_SPI_SLOT slot) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, round, frame_buffer, PIMORONI_SPI_DEFAULT_INSTANCE, st7789.get_slot_cs(slot), SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, st7789.get_slot_bl(slot)) {
            this->frame_buffer = (Pen *)st7789.frame_buffer;
            this->st7789.init();
           };

    ST7789Generic(uint16_t width, uint16_t height, bool round, void *frame_buffer,
           spi_inst_t *spi,
           uint cs, uint dc, uint sck, uint mosi, uint bl = PIN_UNUSED) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, round, frame_buffer, spi, cs, dc, sck, mosi, bl) {
            this->frame_buffer = (Pen *)st7789.frame_buffer;
            this->st7789.init();
           };

    ST7789Generic(uint16_t width, uint16_t height, void *frame_buffer,
           uint cs, uint dc, uint wr_sck, uint rd_sck, uint d0, uint bl = PIN_UNUSED) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, frame_buffer, cs, dc, wr_sck, rd_sck, d0, bl) {
            this->frame_buffer = (Pen *)st7789.frame_buffer;
            this->st7789.init();
           };

    spi_inst_t* get_spi() const;
    int get_cs() const;
    int get_dc() const;
    int get_sck() const;
    int get_mosi() const;
    int get_bl() const;

    void update();
    [[deprecated("Use configure_display(true) instead.")]] void flip();
    void set_backlight(uint8_t brightness);
    void configure_display(bool rotate180);
  };

}
