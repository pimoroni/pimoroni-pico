#pragma once

#include <initializer_list>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/shiftregister/shiftregister.hpp"

namespace pimoroni {

  class Inky73 : public DisplayDriver {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    // interface pins with our standard defaults where appropriate
    uint CS     = SPI_BG_FRONT_CS;
    uint DC     = 28; // 27;
    uint SCK    = SPI_DEFAULT_SCK;
    uint MOSI   = SPI_DEFAULT_MOSI;
    uint RESET  = 27; //25;

    uint SR_CLOCK = 8;
    uint SR_LATCH = 9;
    uint SR_DATA = 10;

    bool blocking = false;

    ShiftRegister<uint8_t> sr = ShiftRegister<uint8_t>(SR_CLOCK, SR_LATCH, SR_DATA);

  public:
    enum Button : uint8_t {
      BUTTON_A = 1,
      BUTTON_B = 2,
      BUTTON_C = 4,
      BUTTON_D = 8,
      BUTTON_E = 16
    };
  
    enum colour : uint8_t {
      BLACK = 0,
      WHITE = 1,
      GREEN = 2,
      BLUE = 3,
      RED = 4,
      YELLOW = 5,
      ORANGE = 6,
      CLEAN = 7
    };

    Inky73(uint16_t width, uint16_t height) : Inky73(width, height, ROTATE_0, {PIMORONI_SPI_DEFAULT_INSTANCE, SPI_BG_FRONT_CS, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 28, PIN_UNUSED}) {};

    Inky73(uint16_t width, uint16_t height, SPIPins pins, uint reset=27) : Inky73(width, height, ROTATE_0, pins, reset) {};

    Inky73(uint16_t width, uint16_t height, Rotation rotation, SPIPins pins, uint reset=27) :
      DisplayDriver(width, height, rotation),
      spi(pins.spi),
      CS(pins.cs), DC(pins.dc), SCK(pins.sck), MOSI(pins.mosi), RESET(reset) {
        init();
      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void busy_wait();
    void reset();
    void power_off();
  
    bool is_busy() override;
    void update(PicoGraphics *graphics) override;

    void set_blocking(bool blocking);

    bool is_pressed(Button button);

  private:
    void init();
    void setup();
    void command(uint8_t reg, size_t len, const uint8_t *data);
    void command(uint8_t reg, std::initializer_list<uint8_t> values);
    void command(uint8_t reg, const uint8_t data) {command(reg, 0, &data);};
    void command(uint8_t reg) {command(reg, 0, nullptr);};
    void data(size_t len, const uint8_t *data);
  };

}
