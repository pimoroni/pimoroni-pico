#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"


namespace pimoroni {


  class XPT2046 : public TouchDriver {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;
  
  public:

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint cs;
    uint irq;
    uint16_t transfer_16(uint8_t cmd);
  
  public:
    XPT2046(uint16_t width, uint16_t height, Rotation rotation, SPIPins pins, uint irq_pin, uint baud_rate = 2500000) :
      TouchDriver(width, height, rotation),
      spi(pins.spi), cs(pins.cs), irq(irq_pin) {

      // configure spi interface and pins
      spi_init(spi, baud_rate);

      gpio_set_function(pins.mosi, GPIO_FUNC_SPI);
      gpio_set_function(pins.miso, GPIO_FUNC_SPI);
      gpio_set_function(pins.sck, GPIO_FUNC_SPI);

      gpio_init(cs);
      gpio_set_dir(cs, GPIO_OUT);
      gpio_put(cs, 1);

      gpio_init(irq);
      gpio_set_dir(irq, GPIO_IN);
      gpio_pull_down(irq);

      // calibrate touchscreen roughly to known values
      calibrate_touchscreen(TouchPoint(612, 636), TouchPoint(3594, 3685), 6428, 15688, 20);
    }
  
    virtual ~XPT2046()
    {
    }

    void update(uint16_t average_samples = 16) override;

    void cleanup() override;
  };
}
