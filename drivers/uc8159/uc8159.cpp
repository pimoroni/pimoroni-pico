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
    if(BUSY == PIN_UNUSED) {
      if(absolute_time_diff_us(get_absolute_time(), timeout) > 0) {
        return true;
      } else {
        return false;
      }
    }
    return !gpio_get(BUSY);
  }

  void UC8159::busy_wait(uint minimum_wait_ms) {
    timeout = make_timeout_time_ms(minimum_wait_ms);
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
  };

  void UC8159::setup() {
    reset();
    busy_wait();

    uint8_t dimensions[4] = {
      uint8_t(width >> 8),
      uint8_t(width),
      uint8_t(height >> 8),
      uint8_t(height)
    };

    if (width == 600) {
      if (rotation == ROTATE_0) {
        command(PSR,  {0xE3, 0x08});
      } else {
        command(PSR,  {0xEF, 0x08});
      }
    } else {
      if (rotation == ROTATE_0) {
        command(PSR,  {0xA3, 0x08});
      } else {
        command(PSR,  {0xAF, 0x08});
      }
    }
    command(PWR,  {0x37, 0x00, 0x23, 0x23});
    command(PFS,  {0x00});
    command(BTST, {0xC7, 0xC7, 0x1D});
    command(PLL,  {0x3C});
    command(TSC,  {0x00});
    command(CDI,  {0x37});
    command(TCON, {0x22});
    command(TRES, 4, dimensions);
    command(PWS,  {0xAA});

    sleep_ms(100);

    command(0x50, {0x37});
  }

  void UC8159::set_blocking(bool blocking) {
    this->blocking = blocking;
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

  void UC8159::update(PicoGraphics *graphics) {
    if(graphics->pen_type != PicoGraphics::PEN_3BIT) return; // Incompatible buffer

    if(blocking) {
      busy_wait();
    }

    setup();

    gpio_put(CS, 0);

    uint8_t reg = DTM1;
    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    gpio_put(DC, 1); // data mode

    // HACK: Output 48 rows of data since our buffer is 400px tall
    // but the display has no offset configuration and H/V scan 
    // are reversed.
    // Any garbage data will do.
    // 2px per byte, so we need width * 24 bytes
    if(height == 400 && rotation == ROTATE_0) {
      spi_write_blocking(spi, (uint8_t *)graphics->frame_buffer, width * 24);
    }
    graphics->frame_convert(PicoGraphics::PEN_P4, [this](void *buf, size_t length) {
      if (length > 0) {
        spi_write_blocking(spi, (const uint8_t*)buf, length);
      }
    });

    gpio_put(CS, 1);

    busy_wait();

    command(PON); // turn on
    busy_wait(200);

    command(DRF); // start display refresh
    busy_wait(200);

    if(blocking) {
      busy_wait(32 * 1000);

      command(POF); // turn off
    } else {
      timeout = make_timeout_time_ms(32 * 1000);
    }
  }

}
