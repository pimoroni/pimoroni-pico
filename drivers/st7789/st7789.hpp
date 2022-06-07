#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

#include <algorithm>


namespace pimoroni {

  class ST7789 : public DisplayDriver {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;
  
  public:
    bool round;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint wr_sck;
    uint rd_sck = PIN_UNUSED;
    uint d0;
    uint bl;
    uint vsync  = PIN_UNUSED; // only available on some products

    // The ST7789 requires 16 ns between SPI rising edges.
    // 16 ns = 62,500,000 Hz
    static const uint32_t SPI_BAUD = 62'500'000;


  public:
    // Parallel init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, ParallelPins pins) :
      DisplayDriver(width, height, rotation),
      spi(nullptr), round(false),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.wr_sck), rd_sck(pins.rd_sck), d0(pins.d0), bl(pins.bl) {
  
      gpio_set_function(wr_sck, GPIO_FUNC_SIO);
      gpio_set_dir(wr_sck, GPIO_OUT);

      gpio_set_function(rd_sck,  GPIO_FUNC_SIO);
      gpio_set_dir(rd_sck, GPIO_OUT);

      for(auto i = 0u; i < 8; i++) {
        gpio_set_function(d0 + i, GPIO_FUNC_SIO);
        gpio_set_dir(d0 + i, GPIO_OUT);
      }
  
      gpio_put(rd_sck, 1);

      common_init();
    }

    // Serial init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, SPIPins pins) :
      DisplayDriver(width, height, rotation),
      spi(pins.spi), round(round),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.sck), d0(pins.mosi), bl(pins.bl) {

      // configure spi interface and pins
      spi_init(spi, SPI_BAUD);

      gpio_set_function(wr_sck, GPIO_FUNC_SPI);
      gpio_set_function(d0, GPIO_FUNC_SPI);

      common_init();
    }


    void update(PicoGraphics *graphics) override;
    void set_backlight(uint8_t brightness) override;

  private:
    void common_init();
    void configure_display(Rotation rotate);
    void write_blocking_parallel(const uint8_t *src, size_t len);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
