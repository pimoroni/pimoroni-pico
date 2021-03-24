#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class ST7789 {
    spi_inst_t *spi = spi0;

    uint32_t dma_channel;

    // screen properties
    uint16_t width;
    uint16_t height;
    uint16_t row_stride;

    // interface pins with our standard defaults where appropriate
    int8_t cs     = 17;
    int8_t dc     = 16;
    int8_t sck    = 18;
    int8_t mosi   = 19;
    int8_t miso   = -1; // we generally don't use this pin
    int8_t bl     = 21;
    int8_t vsync  = -1; // only available on some products

    uint32_t spi_baud = 64 * 1024 * 1024;

  public:
    // frame buffer where pixel data is stored
    uint16_t *frame_buffer;

  public:
    ST7789(uint16_t width, uint16_t height, uint16_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {}

    ST7789(uint16_t width, uint16_t height, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1) :
      spi(spi),
      width(width), height(height),      
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso), frame_buffer(frame_buffer) {}

    void init(bool auto_init_sequence = true, bool round = false);

    void command(uint8_t command, size_t len = 0, const char *data = NULL);
    void vsync_callback(gpio_irq_callback_t callback);
    void update(bool dont_block = false);
    void set_backlight(uint8_t brightness);
    void flip();

    enum MADCTL : uint8_t {
      ROW_ORDER   = 0b10000000,
      COL_ORDER   = 0b01000000,
      SWAP_XY     = 0b00100000,  // AKA "MV"
      SCAN_ORDER  = 0b00010000,
      RGB         = 0b00001000,
      HORIZ_ORDER = 0b00000100
    };

    #define ROT_240_240_0      0
    #define ROT_240_240_90     MADCTL::SWAP_XY | MADCTL::HORIZ_ORDER | MADCTL::COL_ORDER
    #define ROT_240_240_180    MADCTL::SCAN_ORDER | MADCTL::HORIZ_ORDER | MADCTL::COL_ORDER | MADCTL::ROW_ORDER
    #define ROT_240_240_270    MADCTL::SWAP_XY | MADCTL::HORIZ_ORDER | MADCTL::ROW_ORDER

    enum reg {
      SWRESET   = 0x01,
      TEON      = 0x35,
      MADCTL    = 0x36,
      COLMOD    = 0x3A,
      GCTRL     = 0xB7,
      VCOMS     = 0xBB,
      LCMCTRL   = 0xC0,
      VDVVRHEN  = 0xC2,
      VRHS      = 0xC3,
      VDVS      = 0xC4,
      FRCTRL2   = 0xC6,
      PWRCTRL1  = 0xD0,
      FRMCTR1   = 0xB1,
      FRMCTR2   = 0xB2,
      GMCTRP1   = 0xE0,
      GMCTRN1   = 0xE1,
      INVOFF    = 0x20,
      SLPOUT    = 0x11,
      DISPON    = 0x29,
      GAMSET    = 0x26,
      DISPOFF   = 0x28,
      RAMWR     = 0x2C,
      INVON     = 0x21,
      CASET     = 0x2A,
      RASET     = 0x2B
    };
  };

}
