#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class ST7789Generic : public PicoGraphics {
  private:
    ST7789 st7789;

  public:
    ST7789Generic(int width, int height, BG_SPI_SLOT slot, uint16_t *frame_buffer = nullptr);
    ST7789Generic(uint16_t width, uint16_t height, spi_inst_t *spi,
           uint cs, uint dc, uint sck, uint mosi, uint miso = PIN_UNUSED, uint bl = PIN_UNUSED,
            uint16_t *frame_buffer = nullptr);

    spi_inst_t* get_spi() const;
    int get_cs() const;
    int get_dc() const;
    int get_sck() const;
    int get_mosi() const;
    int get_bl() const;

    void update();
    void flip();
    void set_backlight(uint8_t brightness);
  };

}
