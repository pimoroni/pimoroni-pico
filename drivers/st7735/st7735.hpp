#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class ST7735 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_CS_PIN     = 17;
    static const uint8_t DEFAULT_DC_PIN     = 16;
    static const uint8_t DEFAULT_SCK_PIN    = 18;
    static const uint8_t DEFAULT_MOSI_PIN   = 19;
    static const uint8_t DEFAULT_BL_PIN     = 20;

  private:
    static const uint8_t ROWS = 162;
    static const uint8_t COLS = 132;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;
    uint16_t row_stride;

  public:
    // frame buffer where pixel data is stored
    uint16_t *frame_buffer;

  private:
    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // interface pins with our standard defaults where appropriate
    uint cs     = DEFAULT_CS_PIN;
    uint dc     = DEFAULT_DC_PIN;
    uint sck    = DEFAULT_SCK_PIN;
    uint mosi   = DEFAULT_MOSI_PIN;
    uint miso   = PIN_UNUSED; // we generally don't use this pin
    uint bl     = DEFAULT_BL_PIN;
    uint vsync  = PIN_UNUSED; // only available on some products

    uint32_t spi_baud = 30 * 1024 * 1024;

    uint8_t offset_cols = 0;
    uint8_t offset_rows = 0;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer, BG_SPI_SLOT slot) :
      width(width), height(height), frame_buffer(frame_buffer) {
      switch(slot) {
        case PICO_EXPLORER_ONBOARD: // Don't read too much into this, the case is just here to avoid a compile error
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

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {}

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1, uint8_t bl = -1) :
      width(width), height(height), frame_buffer(frame_buffer),
      spi(spi), cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso), bl(bl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init(bool auto_init_sequence = true);

    spi_inst_t* get_spi() const;
    int get_cs() const;
    int get_dc() const;
    int get_sck() const;
    int get_mosi() const;
    int get_bl() const;
    
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
    void update(bool dont_block = false);
    void set_backlight(uint8_t brightness);
  };

}
