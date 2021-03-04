#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class ST7735 {
    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // screen properties
    uint16_t width = 160;
    uint16_t height = 80;
    uint16_t row_stride;

    // interface pins with our standard defaults where appropriate
    int8_t cs     = 17;
    int8_t dc     = 16;
    int8_t sck    = 18;
    int8_t mosi   = 19;
    int8_t miso   = -1; // we generally don't use this pin
    int8_t bl     = 20;
    int8_t vsync  = -1; // only available on some products

    uint32_t spi_baud = 30 * 1024 * 1024;


    private:
      static const uint8_t ROWS = 162;
      static const uint8_t COLS = 132;

  public:
    // frame buffer where pixel data is stored
    uint16_t *frame_buffer;
    uint8_t offset_cols = 0;
    uint8_t offset_rows = 0;

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {}

    ST7735(uint16_t width, uint16_t height, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1) :
      spi(spi),
      width(width), height(height),      
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso), frame_buffer(frame_buffer) {}

    void init(bool auto_init_sequence = true);

    void command(uint8_t command, size_t len = 0, const char *data = NULL);
    void update(bool dont_block = false);
    void set_backlight(uint8_t brightness);

    enum reg : uint8_t {
      SWRESET   = 0x01,
      RDDID     = 0x04,
      RDDRST    = 0x09,
      RDDPM     = 0x0A,
      RDDMADCTL = 0x0B,
      RDDCOLMOD = 0x0C,
      RDDIM     = 0x0D,
      RDDSM     = 0x0E,
      RDDSDR    = 0x0F,
      SLPIN     = 0x10,
      SLPOUT    = 0x11,
      PTLON     = 0x12,
      NORON     = 0x13,
      INVOFF    = 0x20,
      INVON     = 0x21,
      GAMSET    = 0x26,
      DISPOFF   = 0x28,
      DISPON    = 0x29,
      CASET     = 0x2A,
      RASET     = 0x2B,
      RAMWR     = 0x2C,
      RGBSET    = 0x2D,
      RAMRD     = 0x2E,
      PTLAR     = 0x30,
      SCRLAR    = 0x33,
      TEOFF     = 0x34,
      TEON      = 0x35,
      MADCTL    = 0x36,  // Memory Data Access Control
      VSCSAD    = 0x37,
      IDMOFF    = 0x38,  // Idle Mode Off
      IDMON     = 0x39,  // Idle Mode On
      COLMOD    = 0x3A,

      FRMCTR1   = 0xB1,
      FRMCTR2   = 0xB2,
      FRMCTR3   = 0xB3,
      INVCTR    = 0xB4,
      DISSET5   = 0xB6,

      PWCTR1    = 0xC0,
      PWCTR2    = 0xC1,
      PWCTR3    = 0xC2,
      PWCTR4    = 0xC3,
      PWCTR5    = 0xC4,
      VMCTR1    = 0xC5,

      RDID1     = 0xDA,
      RDID2     = 0xDB,
      RDID3     = 0xDC,
      RDID4     = 0xDD,

      GMCTRP1   = 0xE0,
      GMCTRN1   = 0xE1,

      PWMTR6    = 0xFC
    };
  };

}
