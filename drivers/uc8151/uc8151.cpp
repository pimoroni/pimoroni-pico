#include "uc8151.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {

  enum reg {
    PSR      = 0x00,
    PWR      = 0x01,
    POF      = 0x02,
    PFS      = 0x03,
    PON      = 0x04,
    PMES     = 0x05,
    BTST     = 0x06,
    DSLP     = 0x07,
    DTM1     = 0x10,
    DSP      = 0x11,
    DRF      = 0x12,
    DTM2     = 0x13,
    LUT_VCOM = 0x20,
    LUT_WW   = 0x21,
    LUT_BW   = 0x22,
    LUT_WB   = 0x23,
    LUT_BB   = 0x24,
    PLL      = 0x30,
    TSC      = 0x40,
    TSE      = 0x41,
    TSR      = 0x43,
    TSW      = 0x42,
    CDI      = 0x50,
    LPD      = 0x51,
    TCON     = 0x60,
    TRES     = 0x61,
    REV      = 0x70,
    FLG      = 0x71,
    AMV      = 0x80,
    VV       = 0x81,
    VDCS     = 0x82,
    PTL      = 0x90,
    PTIN     = 0x91,
    PTOU     = 0x92,
    PGM      = 0xa0,
    APG      = 0xa1,
    ROTP     = 0xa2,
    CCSET    = 0xe0,
    PWS      = 0xe3,
    TSSET    = 0xe5
  };

  bool UC8151::is_busy() {
    return !gpio_get(BUSY);
  }

  void UC8151::busy_wait() {
    while(is_busy()) {
      tight_loop_contents();
    }
  }

  void UC8151::reset() {
    gpio_put(RESET, 0); sleep_ms(10);
    gpio_put(RESET, 1); sleep_ms(10);
    busy_wait();
  }

  void UC8151::init() {
    // configure spi interface and pins
    spi_init(spi, 12'000'000);

    gpio_set_function(DC, GPIO_FUNC_SIO);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_set_function(CS, GPIO_FUNC_SIO);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);

    gpio_set_function(RESET, GPIO_FUNC_SIO);
    gpio_set_dir(RESET, GPIO_OUT);
    gpio_put(RESET, 1);

    gpio_set_function(BUSY, GPIO_FUNC_SIO);
    gpio_set_dir(BUSY, GPIO_IN);
    gpio_set_pulls(BUSY, true, false);

    gpio_set_function(SCK,  GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);

    reset();

    command(PSR, {
      RES_128x296 | LUT_OTP | FORMAT_BW | SHIFT_RIGHT | BOOSTER_ON | RESET_NONE
    });

    /*
      uint8_t lut[44];

      for(int i = 0; i < 7; i++) {
        //lut[i * 6 + 0] = 0b10'10'10'10; // level selection
        lut[i * 6 + 0] = 0b10'10'10'10; // level selection
        lut[i * 6 + 1] = 1; // number of frames 0
        lut[i * 6 + 2] = 1; // number of frames 0
        lut[i * 6 + 3] = 1; // number of frames 0
        lut[i * 6 + 4] = 1; // number of frames 0
        lut[i * 6 + 5] = 1; // repeat count
      }
      command(LUT_BW, 42, lut);

      for(int i = 0; i < 7; i++) {
        //lut[i * 6 + 0] = 0b01'01'01'01; // level selection
        lut[i * 6 + 0] = 0b01'01'01'01; // level selection
        lut[i * 6 + 1] = 1; // number of frames 0
        lut[i * 6 + 2] = 1; // number of frames 0
        lut[i * 6 + 3] = 1; // number of frames 0
        lut[i * 6 + 4] = 1; // number of frames 0
        lut[i * 6 + 5] = 1; // repeat count
      }
      command(LUT_WB, 42, lut);

      for(int i = 0; i < 7; i++) {
    //    lut[i * 6 + 0] = 0b00'00'00'00; // level selection
        lut[i * 6 + 0] = 0b00'00'00'00; // level selection
        lut[i * 6 + 1] = 1; // number of frames 0
        lut[i * 6 + 2] = 1; // number of frames 0
        lut[i * 6 + 3] = 1; // number of frames 0
        lut[i * 6 + 4] = 1; // number of frames 0
        lut[i * 6 + 5] = 1; // repeat count
      }
      command(LUT_WW, 42, lut);

      for(int i = 0; i < 7; i++) {
    //    lut[i * 6 + 0] = 0b11'11'11'11; // level selection
        lut[i * 6 + 0] = 0b11'11'11'11; // level selection
        lut[i * 6 + 1] = 1; // number of frames 0
        lut[i * 6 + 2] = 1; // number of frames 0
        lut[i * 6 + 3] = 1; // number of frames 0
        lut[i * 6 + 4] = 1; // number of frames 0
        lut[i * 6 + 5] = 1; // repeat count
      }
      command(LUT_BB, 42, lut);


    */

    command(PWR, {
      VDS_INTERNAL | VDG_INTERNAL,
      VCOM_VD | VGHL_16V,
      0b101011,
      0b101011,
      0b101011
    });

    command(PON); // power on
    busy_wait();

    // booster soft start configuration
    command(BTST, {
      START_10MS | STRENGTH_3 | OFF_6_58US,
      START_10MS | STRENGTH_3 | OFF_6_58US,
      START_10MS | STRENGTH_3 | OFF_6_58US
    });

    command(PFS, {
      FRAMES_1
    });

    command(TSE, {
      TEMP_INTERNAL | OFFSET_0
    });

    command(TCON, {0x22}); // tcon setting
    command(CDI, {0b01'00'1100}); // vcom and data interval

    command(PLL, {
      HZ_100
    });

    command(POF);
    busy_wait();
  }

  void UC8151::command(uint8_t reg, size_t len, const uint8_t *data) {
    gpio_put(CS, 0);

    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    if(len > 0) {
      gpio_put(DC, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(CS, 1);
  }

  void UC8151::data(size_t len, const uint8_t *data) {
    gpio_put(CS, 0);
    gpio_put(DC, 1); // data mode
    spi_write_blocking(spi, (const uint8_t*)data, len);
    gpio_put(CS, 1);
  }

  void UC8151::command(uint8_t reg, std::initializer_list<uint8_t> values) {
    command(reg, values.size(), (uint8_t *)values.begin());
  }

  void UC8151::pixel(int x, int y, int v) {
    // bounds check
    if(x < 0 || y < 0 || x >= width || y >= height) return;

    // pointer to byte in framebuffer that contains this pixel
    uint8_t *p = &frame_buffer[(y / 8) + (x * (height / 8))];

    uint8_t  o = 7 - (y & 0b111);       // bit offset within byte
    uint8_t  m = ~(1 << o);             // bit mask for byte
    uint8_t  b = (v == 0 ? 0 : 1) << o; // bit value shifted to position

    *p &= m; // clear bit
    *p |= b; // set bit value
  }

  void UC8151::partial_update(int x, int y, int w, int h, bool blocking) {
    // y is given in columns ("banks"), which are groups of 8 horiontal pixels
    // x is given in pixels

    int cols = h / 8;
    int y1 = y / 8;
    //int y2 = y1 + cols;

    int rows = w;
    int x1 = x;
    //int x2 = x + rows;

    uint8_t partial_window[7] = {
      (uint8_t)(y),
      (uint8_t)(y + h - 1),
      (uint8_t)(x >> 8),
      (uint8_t)(x & 0xff),
      (uint8_t)((x + w - 1) >> 8),
      (uint8_t)((x + w - 1) & 0xff),
      0b00000001  // PT_SCAN
    };
    command(PON); // turn on
    command(PTIN); // enable partial mode
    command(PTL, sizeof(partial_window), partial_window);

    command(DTM2);
    for (auto dx = 0; dx < rows; dx++) {
      int sx = dx + x1;
      int sy = y1;
      data(cols, &frame_buffer[sy + (sx * (height / 8))]);
    }
    command(DSP); // data stop

    command(DRF); // start display refresh

    if(blocking) {
      busy_wait();

      command(POF); // turn off
    }
    command(PTOU); // disable partial mode
  }

  void UC8151::update(bool blocking) {
    command(PON); // turn on

    command(DTM2, (width * height) / 8, frame_buffer); // transmit framebuffer
    command(DSP); // data stop

    command(DRF); // start display refresh

    if(blocking) {
      busy_wait();

      command(POF); // turn off
    }
  }

  void UC8151::off() {
    busy_wait();
    command(POF); // turn off
  }

}
