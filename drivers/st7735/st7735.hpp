#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class ST7735 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint8_t ROWS = 162;
    static const uint8_t COLS = 132;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;

    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint sck;
    uint mosi;
    uint bl;

    uint32_t spi_baud = 30 * 1024 * 1024;

    uint8_t offset_cols = 0;
    uint8_t offset_rows = 0;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ST7735(uint16_t width, uint16_t height, SPIPins pins) :
      width(width), height(height),
      spi(pins.spi), cs(pins.cs), dc(pins.dc), sck(pins.sck), mosi(pins.mosi), bl(pins.bl) {
        init();
      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    // Update functions for supported Pen types
    void update(PicoGraphics<PenRGB565> *graphics);
    void update(PicoGraphics<PenRGB332> *graphics);
    void set_backlight(uint8_t brightness);

  private:
    void init(bool auto_init_sequence = true);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
