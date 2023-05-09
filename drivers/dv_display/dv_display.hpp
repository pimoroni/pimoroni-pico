#pragma once

#include <cstring>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"
#include "drivers/aps6404/aps6404.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"


// Pimoroni DV Stick

namespace pimoroni {

  // This is ARGB1555 only for now
  class DVDisplay : public IDirectDisplayDriver<uint16_t> {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // Ram accessed through the APS6404 driver
    APS6404 ram;

    // I2C interface to driver
    I2C i2c;

    // interface pins
    static constexpr uint CS     = 17;
    static constexpr uint D0     = 19;
    static constexpr uint VSYNC  = 16;
    static constexpr uint RAM_SEL = 8;
    static constexpr uint I2C_SDA = 6;
    static constexpr uint I2C_SCL = 7;

    // I2C
    static constexpr uint I2C_ADDR = 0x0D;
    static constexpr uint I2C_REG_START = 0xF9;
    
    static constexpr uint32_t base_address = 0x10000;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t bank = 0;

  public:
    DVDisplay(uint16_t width, uint16_t height) 
      : ram(CS, D0)
      , i2c(I2C_SDA, I2C_SCL)
      , width(width), height(height)
    {}

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
    public:
      void test(void){
        char writeBuffer[256];
        char readBuffer[256];

        uint mb = 8;

        for (uint b = 0; b < 2; ++b) {
          gpio_put(RAM_SEL, b);

          for(uint k = 0; k < 1024*mb; k++) {
            sprintf(writeBuffer, "%u-%u", b, k);

            write(k*1024, strlen(writeBuffer)+1, (uint16_t *)writeBuffer);
          }
        }

        for (uint b = 0; b < 2; ++b) {
          gpio_put(RAM_SEL, b);
          bool bSame = true;
          for(uint k = 0; k < 1024*mb && bSame; k++)
          {
            sprintf(writeBuffer, "%u-%u", b, k);
            read(k*1024, strlen(writeBuffer)+1, (uint16_t *)readBuffer);
            bSame = strcmp(writeBuffer, readBuffer) == 0;
            printf("[%u] %s == %s ? %s\n", k, writeBuffer, readBuffer, bSame ? "Success" : "Failure");
          }
        }
      }
      
      void write_pixel(const Point &p, uint16_t colour) override;
      void write_pixel_span(const Point &p, uint l, uint16_t colour) override;
      void read_pixel_span(const Point &p, uint l, uint16_t *data) override;

      void init();
      void flip();

    private:
      void write(uint32_t address, size_t len, const uint16_t *data);
      void write(uint32_t address, size_t len, const uint16_t byte);
      void read(uint32_t address, size_t len, uint16_t *data);

      void write_header(uint bank);

      uint32_t pointToAddress(const Point &p)
      {
        return base_address + ((p.y * (uint32_t)width) + p.x) * 2;
      }
  };
}
