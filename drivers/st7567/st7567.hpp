#pragma once

#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {
  class ST7567 : public DisplayDriver {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint8_t ROWS = 64;
    static const uint8_t COLS = 128;
    static const uint8_t PAGESIZE = 128;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint sck;
    uint mosi;
    uint bl;
    uint reset_pin=21;

    uint32_t spi_baud = 10000000; //10Mhz

    uint8_t offset_cols = 0;
    uint8_t offset_rows = 0;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:

    ST7567(uint16_t width, uint16_t height, SPIPins pins) :
      DisplayDriver(width, height, ROTATE_0),
      spi(pins.spi), cs(pins.cs), dc(pins.dc), sck(pins.sck), mosi(pins.mosi), bl(pins.bl) {
      init();
    }
      

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void update(PicoGraphics *graphics) override;
    void set_backlight(uint8_t brightness) override;
    void reset();

  private:
    void init(bool auto_init_sequence = true);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
