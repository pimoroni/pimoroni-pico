#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class ST7567 : public DisplayDriver {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint8_t ROWS = 128;
    static const uint8_t COLS = 64;
    static const uint8_t PAGESIZE = 128;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:

    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // interface pins with our standard defaults where appropriate
    uint cs     = SPI_BG_FRONT_CS;
    uint dc     = 20;
    uint sck    = SPI_DEFAULT_SCK ;
    uint mosi   = SPI_DEFAULT_MOSI;
    uint bl     = PIN_UNUSED;
    uint reset  = PIN_UNUSED;

    uint32_t spi_baud = 30 * 1024 * 1024;

    uint8_t offset_cols = 0;
    uint8_t offset_rows = 0;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ST7567(uint16_t width, uint16_t height, Rotation rotate) : ST7567(width, height, rotate, {PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, PIN_UNUSED}) {};

    ST7567(uint16_t width, uint16_t height, SPIPins pins, uint busy=26, uint reset=21) :
      DisplayDriver(width, height, ROTATE_0),
      spi(pins.spi),
      CS(pins.cs), DC(pins.dc), SCK(pins.sck), MOSI(pins.mosi), BUSY(busy), RESET(reset) {
        init();
      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void update(PicoGraphics *graphics) override;
    void set_backlight(uint8_t brightness) override;

  private:
    void init(bool auto_init_sequence = true);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
