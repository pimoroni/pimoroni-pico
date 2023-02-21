#pragma once

#include <cstring>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"


// 8 MB PSRam

namespace pimoroni {

  class PSRamDisplay : public IDirectDisplayDriver<uint8_t> {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    // interface pins with our standard defaults where appropriate
    uint CS     = 3;
    uint DC     = PIN_UNUSED;
    uint SCK    = SPI_DEFAULT_SCK;
    uint MOSI   = SPI_DEFAULT_MOSI;
    uint MISO   = SPI_DEFAULT_MISO;
    
    uint32_t start_address = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    
    absolute_time_t timeout;

    bool blocking = false;

  public:
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

    PSRamDisplay(uint16_t width, uint16_t height) : PSRamDisplay(width, height, {PIMORONI_SPI_DEFAULT_INSTANCE, 3, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, PIN_UNUSED, PIN_UNUSED}) {};

    PSRamDisplay(uint16_t width, uint16_t height, SPIPins pins) :
      spi(pins.spi),
      CS(pins.cs), DC(pins.dc), SCK(pins.sck), MOSI(pins.mosi),
      width(width), height(height) {
        init();
      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
    public:
      void test(void){

        char writeBuffer[1024];
        char readBuffer[1024];

        uint mb = 8;

        for(uint k = 0; k < 1024*mb; k++)
        {
          sprintf(writeBuffer, "%u", k);

          write(k*1024, strlen(writeBuffer)+1, (uint8_t *)writeBuffer);
        }

        bool bSame = true;
        for(uint k = 0; k < 1024*mb && bSame; k++)
        {
          sprintf(writeBuffer, "%u", k);
          read(k*1024, strlen(writeBuffer)+1, (uint8_t *)readBuffer);
          bSame = strcmp(writeBuffer, readBuffer) ==0;
          printf("[%u] %s == %s ? %s\n", k, writeBuffer, readBuffer, bSame ? "Success" : "Failure");
        }
      }
      
      void write_pixel(const Point &p, uint8_t colour) override;
      void write_pixel_span(const Point &p, uint l, uint8_t colour) override;
      void read_pixel_span(const Point &p, uint l, uint8_t *data) override;

      int __not_in_flash_func(SpiSetBlocking)(const uint16_t uSrc, size_t uLen) 
      {
        invalid_params_if(SPI, 0 > (int)uLen);
        // Deliberately overflow FIFO, then clean up afterward, to minimise amount
        // of APB polling required per halfword
        for (size_t i = 0; i < uLen; ++i) {
            while (!spi_is_writable(spi))
                tight_loop_contents();
            spi_get_hw(spi)->dr = uSrc;
        }

        while (spi_is_readable(spi))
            (void)spi_get_hw(spi)->dr;
        while (spi_get_hw(spi)->sr & SPI_SSPSR_BSY_BITS)
            tight_loop_contents();
        while (spi_is_readable(spi))
            (void)spi_get_hw(spi)->dr;

        // Don't leave overrun flag set
        spi_get_hw(spi)->icr = SPI_SSPICR_RORIC_BITS;

        return (int)uLen;
      }

    private:
      void init();
      void write(uint32_t address, size_t len, const uint8_t *data);
      void write(uint32_t address, size_t len, const uint8_t byte);
      void read(uint32_t address, size_t len, uint8_t *data);

      uint32_t pointToAddress(const Point &p)
      {
        return start_address + (p.y * width) + p.x;
      }
  };
}
