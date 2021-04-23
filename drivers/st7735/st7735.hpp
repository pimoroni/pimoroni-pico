#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class ST7735 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint8_t ROWS = 162;
    static const uint8_t COLS = 132;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum BG_SPI_SLOT {
      BG_SPI_FRONT,
      BG_SPI_BACK
    };


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
    int8_t cs     = 17;
    int8_t dc     = 16;
    int8_t sck    = 18;
    int8_t mosi   = 19;
    int8_t miso   = -1; // we generally don't use this pin
    int8_t bl     = 20;
    int8_t vsync  = -1; // only available on some products

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
        case BG_SPI_FRONT:
          cs = 17;
          bl = 20;
          break;
        case BG_SPI_BACK:
          cs = 22;
          bl = 21;
          break;
      }
    }

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {}

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1, uint8_t bl = -1) :
      width(width), height(height), frame_buffer(frame_buffer) ,
      spi(spi),
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso), bl(bl) {}


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
