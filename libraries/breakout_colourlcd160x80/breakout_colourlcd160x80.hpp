#pragma once

#include "drivers/st7735/st7735.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class BreakoutColourLCD160x80 : public PicoGraphics {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const int WIDTH            = 160;
    static const int HEIGHT           = 80;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    void *__fb;
  private:
    ST7735 screen;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutColourLCD160x80(void *buf);
    BreakoutColourLCD160x80(void *buf, spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint miso = PIN_UNUSED, uint bl = PIN_UNUSED);
    BreakoutColourLCD160x80(void *buf, BG_SPI_SLOT slot);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();

    spi_inst_t* get_spi() const;
    int get_cs() const;
    int get_dc() const;
    int get_sck() const;
    int get_mosi() const;
    int get_bl() const;

    void update();
    void set_backlight(uint8_t brightness);
  };

}
