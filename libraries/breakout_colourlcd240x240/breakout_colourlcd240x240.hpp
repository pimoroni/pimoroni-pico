#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class BreakoutColourLCD240x240 : public PicoGraphics {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 240;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    uint16_t *__fb;
  private:
    ST7789 screen;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutColourLCD240x240(uint16_t *buf);
    BreakoutColourLCD240x240(uint16_t *buf,  spi_inst_t *spi,
      uint cs, uint dc, uint sck, uint mosi, uint miso = PIN_UNUSED, uint bl = PIN_UNUSED);
    BreakoutColourLCD240x240(uint16_t *buf,  BG_SPI_SLOT slot);
    

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
