#pragma once

#include <initializer_list>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class SSD1680 : public DisplayDriver {
    enum RAM_FLAGS {
      X_START    = 0x00,
      X_END      = 0x15,
      Y_START_H  = 0x01,
      Y_START_L  = 0x07,
      Y_END_H    = 0x00,
      Y_END_L    = 0x00 
    };

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    // interface pins with our standard defaults where appropriate
    uint CS     = SPI_BG_FRONT_CS;
    uint DC     = 20;
    uint SCK    = SPI_DEFAULT_SCK;
    uint MOSI   = SPI_DEFAULT_MOSI;
    uint BUSY   = 9;
    uint RESET  = 21;

    bool inverted = true; // Makes 0 black and 1 white, as is foretold.
    bool blocking = true;

  public:
    SSD1680(uint16_t width, uint16_t height, Rotation rotate) : SSD1680(width, height, rotate, {PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 20, PIN_UNUSED}) {};

    SSD1680(uint16_t width, uint16_t height, Rotation rotate, SPIPins pins, uint busy=9, uint reset=21) :
      DisplayDriver(width, height, rotate),
      spi(pins.spi),
      CS(pins.cs), DC(pins.dc), SCK(pins.sck), MOSI(pins.mosi), BUSY(busy), RESET(reset) {
        init();
      }

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void busy_wait();
    void reset();
  
    // DisplayDriver API
    bool is_busy() override;
    void power_off() override;
    void update(PicoGraphics *graphics) override;
    void partial_update(PicoGraphics *graphics, Rect region) override;
  
    // SSD1680 Specific
    void default_luts();

  private:
    void init();
    void setup();
  
    void read(uint8_t reg, size_t len, uint8_t *data);
    void command(uint8_t reg, size_t len, const uint8_t *data);
    void command(uint8_t reg, std::initializer_list<uint8_t> values);
    void command(uint8_t reg) {command(reg, 0, nullptr);};
    void data(size_t len, const uint8_t *data);

    void update(bool blocking = true);
    void partial_update(int x, int y, int w, int h, bool blocking = true);
    void off();
  };

}
