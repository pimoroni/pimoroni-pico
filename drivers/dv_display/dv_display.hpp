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
    static constexpr uint I2C_REG_GPIO = 0xC0;
    static constexpr uint I2C_REG_GPIO_HI = 0xC8;
    
    static constexpr uint32_t base_address = 0x10000;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t bank = 0;

  public:
    DVDisplay(uint16_t width, uint16_t height) 
      : ram(CS, D0)
      , i2c(I2C_SDA, I2C_SCL)
      , width(width), height(height)
      , pixel_buffer_location(-1, -1)
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

            ram.write(k*1024, (uint32_t *)writeBuffer, strlen(writeBuffer)+1);
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

      uint8_t get_driver_gpio();
      uint8_t get_driver_gpio_hi();

      bool is_button_b_pressed() { return (get_driver_gpio() & 0x1) != 0x1; }
      bool is_button_c_pressed() { return (get_driver_gpio() & 0x2) != 0x2; }

    private:
      static constexpr int PIXEL_BUFFER_LEN_IN_WORDS = 16;
      uint32_t pixel_buffer[PIXEL_BUFFER_LEN_IN_WORDS];
      Point pixel_buffer_location;
      int32_t pixel_buffer_x;

      void write(uint32_t address, size_t len, const uint16_t colour);
      void read(uint32_t address, size_t len, uint16_t *data);

      void write_header(uint bank);

      uint32_t pointToAddress(const Point &p)
      {
        return base_address + ((p.y * (uint32_t)width) + p.x) * 2;
      }
  };
}
