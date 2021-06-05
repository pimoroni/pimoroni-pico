#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class ST7789 {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;
    uint16_t row_stride;
    uint32_t dma_channel;

    // interface pins with our standard defaults where appropriate
    uint cs     = SPI_BG_FRONT_CS;
    uint dc     = SPI_DEFAULT_MISO;
    uint sck    = SPI_DEFAULT_SCK;
    uint mosi   = SPI_DEFAULT_MOSI;
    uint miso   = PIN_UNUSED; // used as data/command
    uint bl     = SPI_BG_FRONT_PWM;
    uint vsync  = PIN_UNUSED; // only available on some products

    uint32_t spi_baud = 16 * 1000 * 1000;

  public:
    // frame buffer where pixel data is stored
    uint16_t *frame_buffer;

    ST7789(uint16_t width, uint16_t height, uint16_t *frame_buffer, BG_SPI_SLOT slot) :
      width(width), height(height), frame_buffer(frame_buffer) {
      switch(slot) {
        case PICO_EXPLORER_ONBOARD:
          cs = SPI_BG_FRONT_CS;
          bl = PIN_UNUSED;
          break;
        case BG_SPI_FRONT:
          cs = SPI_BG_FRONT_CS;
          bl = SPI_BG_FRONT_PWM;
          break;
        case BG_SPI_BACK:
          cs = SPI_BG_BACK_CS;
          bl = SPI_BG_BACK_PWM;
          break;
      }
    }

    ST7789(uint16_t width, uint16_t height, uint16_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {}

    ST7789(uint16_t width, uint16_t height, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint cs, uint dc, uint sck, uint mosi, uint miso = PIN_UNUSED, uint bl = PIN_UNUSED) :
      spi(spi),
      width(width), height(height),      
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso), bl(bl), frame_buffer(frame_buffer) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init(bool auto_init_sequence = true, bool round = false);

    spi_inst_t* get_spi() const;
    uint get_cs() const;
    uint get_dc() const;
    uint get_sck() const;
    uint get_mosi() const;
    uint get_bl() const;

    void command(uint8_t command, size_t len = 0, const char *data = NULL);
    void vsync_callback(gpio_irq_callback_t callback);
    void update(bool dont_block = false);
    void set_backlight(uint8_t brightness);
    void flip();
  };

}
