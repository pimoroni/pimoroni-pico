#include "st7735.hpp"

#include <cstdlib>
#include <math.h>

#include "hardware/dma.h"
#include "hardware/pwm.h"

namespace pimoroni {

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

  void ST7735::init(bool auto_init_sequence) {
    spi_init(spi, spi_baud);

    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    // if a backlight pin is provided then set it up for
    // pwm control
    if(bl != PIN_UNUSED) {
      pwm_config cfg = pwm_get_default_config();
      pwm_set_wrap(pwm_gpio_to_slice_num(bl), 65535);
      pwm_init(pwm_gpio_to_slice_num(bl), &cfg, true);
      gpio_set_function(bl, GPIO_FUNC_PWM);
      set_backlight(0); // Turn backlight off initially to avoid nasty surprises
    }

    // if auto_init_sequence then send initialisation sequence
    // for our standard displays based on the width and height
    if(auto_init_sequence) {
      command(reg::SWRESET);

      sleep_ms(150);

      command(reg::SLPOUT);

      sleep_ms(500);

      command(reg::FRMCTR1, 3, "\x01\x2c\x2d");              // Rate = fosc/(1x2+40) * (LINE+2C+2D)
      command(reg::FRMCTR2, 3, "\x01\x2c\x2d");              // Rate = fosc/(1x2+40) * (LINE+2C+2D)
      command(reg::FRMCTR3, 6, "\x01\x2c\x2d\x01\x2c\x2d");  // Rate = fosc/(1x2+40) * (LINE+2C+2D)

      command(reg::INVCTR, 1, "\x07");

      command(reg::PWCTR1, 3, "\xa2\x02\x84");
      command(reg::PWCTR2, 2, "\x0a\x00");
      command(reg::PWCTR4, 2, "\x8a\x2a");
      command(reg::PWCTR5, 2, "\x8a\xee");

      command(reg::VMCTR1, 1, "\x0e");

      // if invert
      // command(reg::INVON)
      // else
      command(reg::INVON);

      command(reg::MADCTL, 1, "\x68"); // 0b0110 1000 (0x68)
      command(reg::COLMOD, 1, "\x05");

      offset_cols = (ROWS - width) / 2;
      offset_rows = (COLS - height) / 2;

      char buf[4];
      buf[0] = 0; 
      buf[1] = offset_cols;
      buf[2] = 0;
      buf[3] = width + offset_cols - 1;
      command(reg::CASET, 4, buf);

      buf[1] = offset_rows;
      buf[3] = height + offset_rows - 1;
      command(reg::RASET, 4, buf);

      command(reg::GMCTRP1, 16, "\x02\x1c\x07\x12\x37\x32\x29\x2d\x29\x25\x2b\x39\x00\x01\x03\x10");
      command(reg::GMCTRN1, 16, "\x03\x1d\x07\x06\x2e\x2c\x29\x2d\x2e\x2e\x37\x3f\x00\x00\x02\x10");

      command(reg::NORON);
      sleep_ms(100);

      command(reg::DISPON);
      sleep_ms(100);
    }

    if(bl != PIN_UNUSED) {
      set_backlight(255); // Turn backlight on now surprises have passed
    }
  }

  void ST7735::command(uint8_t command, size_t len, const char *data) {
    gpio_put(cs, 0);

    gpio_put(dc, 0); // command mode
    spi_write_blocking(spi, &command, 1);

    if(data) {
      gpio_put(dc, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(cs, 1);
  }

  // Native 16-bit framebuffer update
  void ST7735::update(PicoGraphics *graphics) {
    if(graphics->pen_type == PicoGraphics::PEN_RGB565) {
      command(reg::RAMWR, width * height * sizeof(uint16_t), (const char*)graphics->frame_buffer);
    } else {
      command(reg::RAMWR);
      gpio_put(dc, 1); // data mode
      gpio_put(cs, 0);

      graphics->frame_convert(PicoGraphics::PEN_RGB565, [this](void *data, size_t length) {
        if (length > 0) {
          spi_write_blocking(spi, (const uint8_t*)data, length);
        }
      });

      gpio_put(cs, 1);
    }
  }

  void ST7735::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }
}