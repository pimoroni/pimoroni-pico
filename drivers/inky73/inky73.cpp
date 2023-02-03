#include "inky73.hpp"

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
    BTST1 = 0x05,
    BTST2 = 0x06,
    DSLP = 0x07,
    BTST3 = 0x08,
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
    T_VDCS = 0x84,
    AGID = 0x86,
    CMDH = 0xAA,
    CCSET =0xE0,
    PWS = 0xE3,
    TSSET = 0xE6 // E5 or E6
  };

  bool Inky73::is_busy() {
    return !(sr.read() & 128);
  }
  
  void Inky73::busy_wait() {
    while(is_busy()) {
      tight_loop_contents();
    }
  }

  void Inky73::reset() {
    gpio_put(RESET, 0); sleep_ms(10);
    gpio_put(RESET, 1); sleep_ms(10);
    busy_wait();
  }
   
  void Inky73::init() {
    // configure spi interface and pins
    spi_init(spi, 20'000'000);

    gpio_set_function(DC, GPIO_FUNC_SIO);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_set_function(CS, GPIO_FUNC_SIO);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);

    gpio_set_function(RESET, GPIO_FUNC_SIO);
    gpio_set_dir(RESET, GPIO_OUT);
    gpio_put(RESET, 1);

    gpio_set_function(SCK,  GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);
  };

  void Inky73::setup() {
    reset();
    busy_wait();

    command(CMDH, {0x49, 0x55, 0x20, 0x08, 0x09, 0x18});
    command(PWR, {0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A});
    if (rotation == ROTATE_0) {
      command(PSR,  {0x53, 0x69});
    } else {
      command(PSR,  {0x5F, 0x69});
    }
    //command(PSR, {0x5F, 0x69});
    command(PFS, {0x00, 0x54, 0x00, 0x44});
    command(BTST1, {0x40, 0x1F, 0x1F, 0x2C});
    command(BTST2, {0x6F, 0x1F, 0x16, 0x25});
    command(BTST3, {0x6F, 0x1F, 0x1F, 0x22});
    command(IPC, {0x00, 0x04});
    command(PLL, {0x02});
    command(TSE, {0x00});
    command(CDI, {0x3F});
    command(TCON, {0x02, 0x00});
    command(TRES, {0x03, 0x20, 0x01, 0xE0});
    command(VDCS, {0x1E});
    command(T_VDCS, {0x00});
    command(AGID, {0x00});
    command(PWS, {0x2F});
    command(CCSET, {0x00});
    command(TSSET, {0x00});
  }

  void Inky73::set_blocking(bool blocking) {
    this->blocking = blocking;
  }

  void Inky73::power_off() {
    busy_wait();
    command(POF); // turn off
  }

  void Inky73::command(uint8_t reg, size_t len, const uint8_t *data) {
    gpio_put(CS, 0);

    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    if(len > 0) {
      gpio_put(DC, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(CS, 1);
  }

  void Inky73::data(size_t len, const uint8_t *data) {
    gpio_put(CS, 0);
    gpio_put(DC, 1); // data mode
    spi_write_blocking(spi, (const uint8_t*)data, len);
    gpio_put(CS, 1);
  }

  void Inky73::command(uint8_t reg, std::initializer_list<uint8_t> values) {
    command(reg, values.size(), (uint8_t *)values.begin());
  }

  void Inky73::update(PicoGraphics *graphics) {
    if(graphics->pen_type != PicoGraphics::PEN_INKY7) return; // Incompatible buffer

    if(blocking) {
      busy_wait();
    }

    setup();

    gpio_put(CS, 0);

    uint8_t reg = DTM1;
    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    gpio_put(DC, 1); // data mode

    uint totalLength = 0;
    gpio_put(CS, 1);
    graphics->frame_convert(PicoGraphics::PEN_INKY7, [this, &totalLength](void *buf, size_t length) {
      if (length > 0) {
        gpio_put(CS, 0);
        spi_write_blocking(spi, (const uint8_t*)buf, length);
        totalLength += length;
        gpio_put(CS, 1);
      }
    });

    gpio_put(DC, 0); // data mode

    gpio_put(CS, 1);

    busy_wait();

    command(PON, {0}); // turn on
    busy_wait();

    command(DRF, {0}); // start display refresh
    busy_wait();

    if(blocking) {
      busy_wait();

      command(POF); // turn off
    }
  }

  bool Inky73::is_pressed(Button button) {
    return sr.read() & button;
  }

}
