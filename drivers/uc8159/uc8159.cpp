#include "uc8159.hpp"

#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {

  enum reg {
    PSR = 0x00,
    PWR = 0x01,
    POF = 0x02,
    PFS = 0x03,
    PON = 0x04,
    BTST = 0x06,
    DSLP = 0x07,
    DTM1 = 0x10,
    DSP = 0x11,
    DRF = 0x12,
    IPC = 0x13,
    PLL = 0x30,
    TSC = 0x40,
    TSE = 0x41,
    TSW = 0x42,
    TSR = 0x43,
    CDI = 0x50,
    LPD = 0x51,
    TCON = 0x60,
    TRES = 0x61,
    DAM = 0x65,
    REV = 0x70,
    FLG = 0x71,
    AMV = 0x80,
    VV = 0x81,
    VDCS = 0x82,
    PWS = 0xE3,
    TSSET = 0xE5
  };

  bool UC8159::is_busy() {
    return !gpio_get(BUSY);
  }

  void UC8159::busy_wait() {
    while(is_busy()) {
      tight_loop_contents();
    }
  }

  void UC8159::reset() {
    gpio_put(RESET, 0); sleep_ms(10);
    gpio_put(RESET, 1); sleep_ms(10);
    busy_wait();
  }

  void UC8159::init() {
    // configure spi interface and pins
    spi_init(spi, 3'000'000);

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

    memset(frame_buffer, WHITE << 4 | WHITE, width * height / 2);
  };

  void UC8159::setup() {
    reset();
    busy_wait();

    /*
    Resolution Setting
    10bit horizontal followed by a 10bit vertical resolution
    we'll let struct.pack do the work here and send 16bit values
    life is too short for manual bit wrangling
    */
    uint16_t resolution[2] = {
      __builtin_bswap16(width),
      __builtin_bswap16(height)
    };
  
    command(TRES, 4, {
      (uint8_t *)resolution
    });

    /*
    Panel Setting
    0b11000000 = Resolution select, 0b00 = 640x480, our panel is 0b11 = 600x448
    0b00100000 = LUT selection, 0 = ext flash, 1 = registers, we use ext flash
    0b00010000 = Ignore
    0b00001000 = Gate scan direction, 0 = down, 1 = up (default)
    0b00000100 = Source shift direction, 0 = left, 1 = right (default)
    0b00000010 = DC-DC converter, 0 = off, 1 = on
    0b00000001 = Soft reset, 0 = Reset, 1 = Normal (Default)
    0b11 = 600x448
    0b10 = 640x400
    */

    command(PSR, {
      (uint8_t)((width == 640) ? 0b10101111 : 0b11101111),
      0x08 // UC8159 7-colour
    });

    command(PWR, {
      (0x06 << 3) |  // ??? - not documented in UC8159 datasheet
      (0x01 << 2) |  // SOURCE_INTERNAL_DC_DC
      (0x01 << 1) |  // GATE_INTERNAL_DC_DC
      0x01,          // LV_SOURCE_INTERNAL_DC_DC
      0x00,          // VGx_20V
      0x23,          // UC8159_7C
      0x23           // UC8159_7C
    });

    /*
    Set the PLL clock frequency to 50Hz
    0b11000000 = Ignore
    0b00111000 = M
    0b00000111 = N
    PLL = 2MHz * (M / N)
    PLL = 2MHz * (7 / 4)
    PLL = 2,800,000 ???
    */
    command(PLL, 0x3C);

    command(TSE, 0x00);

    /*
    VCOM and Data Interval setting
    0b11100000 = Vborder control (0b001 = LUTB voltage)
    0b00010000 = Data polarity
    0b00001111 = Vcom and data interval (0b0111 = 10, default)
    */
    command(CDI, (1 << 5) | 0x17);

    /*
    Gate/Source non-overlap period
    0b11110000 = Source to Gate (0b0010 = 12nS, default)
    0b00001111 = Gate to Source
    */
    command(TCON, 0x22);

    // Disable externalflash
    command(DAM, 0x00);

    command(PWS, 0xAA);

    command(PFS, 0x00);

    //power_off();
  }

  void UC8159::power_off() {
    busy_wait();
    command(POF); // turn off
  }

  void UC8159::command(uint8_t reg, size_t len, const uint8_t *data) {
    gpio_put(CS, 0);

    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    if(len > 0) {
      gpio_put(DC, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(CS, 1);
  }

  void UC8159::data(size_t len, const uint8_t *data) {
    gpio_put(CS, 0);
    gpio_put(DC, 1); // data mode
    spi_write_blocking(spi, (const uint8_t*)data, len);
    gpio_put(CS, 1);
  }

  void UC8159::command(uint8_t reg, std::initializer_list<uint8_t> values) {
    command(reg, values.size(), (uint8_t *)values.begin());
  }

  void UC8159::pixel(int x, int y, int v) {
    // bounds check
    if(x < 0 || y < 0 || x >= width || y >= height) return;

    // pointer to byte in framebuffer that contains this pixel
    uint8_t *p = &frame_buffer[(x / 2) + (y * width / 2)];

    uint8_t  o = (~x & 0b1) * 4; // bit offset within byte
    uint8_t  m = ~(0b1111 << o); // bit mask for byte
    uint8_t  b = v << o;         // bit value shifted to position

    *p &= m; // clear bits
    *p |= b; // set value
  }

  void UC8159::update(bool blocking) {
    if(blocking) {
      busy_wait();
    }

    setup();

    command(DTM1, (width * height) / 2, frame_buffer); // transmit framebuffer
    busy_wait();

    command(PON); // turn on
    busy_wait();

    command(DRF); // start display refresh
    busy_wait();

    if(blocking) {
      busy_wait();

      command(POF); // turn off
    }
  }

}
