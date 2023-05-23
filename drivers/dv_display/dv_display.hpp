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
  class DVDisplay : public IDirectDisplayDriver<uint16_t>, public IPaletteDisplayDriver {
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
    static constexpr uint I2C_REG_SET_RES = 0xF8;
    static constexpr uint I2C_REG_START = 0xF9;
    static constexpr uint I2C_REG_GPIO = 0xC0;
    static constexpr uint I2C_REG_LED = 0xC1;
    static constexpr uint I2C_REG_GPIO_HI = 0xC8;
    static constexpr uint I2C_REG_GPIO_HI_OUT = 0xC9;
    static constexpr uint I2C_REG_GPIO_HI_OE = 0xCA;
    static constexpr uint I2C_REG_GPIO_HI_PULL_UP = 0xCB;
    static constexpr uint I2C_REG_GPIO_HI_PULL_DOWN = 0xCC;
    static constexpr uint I2C_REG_EDID = 0xED;
    
    static constexpr uint32_t base_address = 0x10000;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t bank = 0;
    uint8_t h_repeat = 1;
    uint8_t v_repeat = 1;

  public:
      static constexpr int PALETTE_SIZE = 32;

    // Valid resolutions are:
    //   640x480 (60Hz), 720x480 (60Hz), 720x400 (70Hz), 720x576 (50Hz)
    //   800x600 (60Hz), 800x480 (60Hz), 800x450 (60Hz), 960x540 (50Hz), 1280x720 (30Hz)
    // Note resolutions on the second line require quite extreme overclocking and may not work on all hardware.
    // Either or both of the horizontal or vertical component of any resolution may be halved.
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
      void write_pixel_span(const Point &p, uint l, uint16_t *data);
      void read_pixel_span(const Point &p, uint l, uint16_t *data) override;

      void init();
      void flip();

      // 32 colour palette mode.  Note that palette entries range from 0-31,
      // but when writing colour values the palette entry is in bits 6-2, so the
      // entry value is effectively multiplied by 4.
      void enable_palette(bool enable);
      void set_palette(RGB888 palette[PALETTE_SIZE]);
      void set_palette_colour(uint8_t entry, RGB888 colour);
      
      void write_palette_pixel(const Point &p, uint8_t colour);
      void write_palette_pixel_span(const Point &p, uint l, uint8_t colour);
      void write_palette_pixel_span(const Point &p, uint l, uint8_t* data);
      void read_palette_pixel_span(const Point &p, uint l, uint8_t *data);

      uint8_t get_gpio();
      uint8_t get_gpio_hi();
      void set_gpio_hi_dir(uint pin, bool output);
      void set_gpio_hi_dir_all(uint8_t output_enables);
      void set_gpio_hi(uint pin, bool on);
      void set_gpio_hi_all(uint8_t vals);
      void set_gpio_hi_pull_up(uint pin, bool on);
      void set_gpio_hi_pull_up_all(uint8_t vals);
      void set_gpio_hi_pull_down(uint pin, bool on);
      void set_gpio_hi_pull_down_all(uint8_t vals);

      bool is_button_b_pressed() { return (get_gpio() & 0x1) != 0x1; }
      bool is_button_c_pressed() { return (get_gpio() & 0x2) != 0x2; }

      // Valid LED levels are from 0-127.
      void set_led_level(uint8_t level);
      void set_led_level(float level) { set_led_level((uint8_t)(level * 127.f)); }
      void set_led_heartbeat();

      // The supplied buffer must be at least 128 bytes long
      void get_edid(uint8_t* edid);

    private:
      uint8_t palette[PALETTE_SIZE * 3] alignas(4);
      bool use_palette_mode = false;
      bool rewrite_header = false;

      static constexpr int PIXEL_BUFFER_LEN_IN_WORDS = 16;
      uint32_t pixel_buffer[PIXEL_BUFFER_LEN_IN_WORDS];
      Point pixel_buffer_location;
      int32_t pixel_buffer_x;

      void write(uint32_t address, size_t len, const uint16_t colour);
      void read(uint32_t address, size_t len, uint16_t *data);
      void write(uint32_t address, size_t len, const uint8_t colour);
      void read(uint32_t address, size_t len, uint8_t *data);

      void write_palette();
      void write_header();

      void i2c_modify_bit(uint8_t reg, uint bit, bool enable);

      uint32_t point_to_address(const Point &p) {
        return base_address + ((p.y * (uint32_t)width) + p.x) * 2;
      }

      uint32_t point_to_address_palette(const Point &p) {
        return base_address + (p.y * (uint32_t)width * 2) + p.x;
      }
  };
}
