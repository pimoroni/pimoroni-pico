#include "st7789.hpp"

#include <cstdlib>
#include <math.h>

#include "hardware/dma.h"
#include "hardware/pwm.h"

namespace pimoroni {
  uint8_t madctl;
  uint16_t caset[2] = {0, 0};
  uint16_t raset[2] = {0, 0};

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
    TEOFF     = 0x34,
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
    PWCTRL1   = 0xD0,
    PORCTRL   = 0xB2,
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
    RASET     = 0x2B,
    PWMFRSEL  = 0xCC
  };

  void ST7789::init(bool auto_init_sequence, bool round, uint32_t spi_baud) {
    // configure spi interface and pins
    spi_init(spi, spi_baud);

    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(sck,  GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    if(miso != PIN_UNUSED) {
      gpio_set_function(miso, GPIO_FUNC_SPI);
    }

    // if supported by the display then the vsync pin is
    // toggled high during vertical blanking period
    if(vsync != PIN_UNUSED) {
      gpio_set_function(vsync, GPIO_FUNC_SIO);
      gpio_set_dir(vsync, GPIO_IN);
      gpio_set_pulls(vsync, false, true);
    }

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

      command(reg::TEON);  // enable frame sync signal if used
      command(reg::COLMOD,    1, "\x05");  // 16 bits per pixel

      if(width == 240 && height == 240) {
        command(reg::PORCTRL, 5, "\x0c\x0c\x00\x33\x33");
        command(reg::GCTRL, 1, "\x14");
        command(reg::VCOMS, 1, "\x37");
        command(reg::LCMCTRL, 1, "\x2c");
        command(reg::VDVVRHEN, 1, "\x01");
        command(reg::VRHS, 1, "\x12");
        command(reg::VDVS, 1, "\x20");
        command(reg::PWCTRL1, 2, "\xa4\xa1");
        command(reg::FRCTRL2, 1, "\x0f");
        command(reg::GMCTRP1, 14, "\xD0\x04\x0D\x11\x13\x2B\x3F\x54\x4C\x18\x0D\x0B\x1F\x23");
        command(reg::GMCTRN1, 14, "\xD0\x04\x0C\x11\x13\x2C\x3F\x44\x51\x2F\x1F\x1F\x20\x23");
      }

      if(width == 320 && height == 240) {
        command(reg::PORCTRL, 5, "\x0c\x0c\x00\x33\x33");
        command(reg::GCTRL, 1, "\x35");
        command(reg::VCOMS, 1, "\x1f");
        command(reg::LCMCTRL, 1, "\x2c");
        command(reg::VDVVRHEN, 1, "\x01");
        command(reg::VRHS, 1, "\x12");
        command(reg::VDVS, 1, "\x20");
        command(reg::FRCTRL2, 1, "\x0f");
        command(reg::PWCTRL1, 2, "\xa4\xa1");
        command(0xd6, 1, "\xa1"); // ???
        command(reg::GMCTRP1, 14, "\xD0\x08\x11\x08\x0C\x15\x39\x33\x50\x36\x13\x14\x29\x2D");
        command(reg::GMCTRN1, 14, "\xD0\x08\x10\x08\x06\x06\x39\x44\x51\x0B\x16\x14\x2F\x31");
      }

      command(reg::INVON);   // set inversion mode
      command(reg::SLPOUT);  // leave sleep mode
      command(reg::DISPON);  // turn display on

      sleep_ms(100);

      // setup correct addressing window
      if(width == 240 && height == 240) {
        caset[0] = 0;
        caset[1] = 239;
        raset[0] = round ? 40 : 0;
        raset[1] = round ? 279 : 239;
        madctl = MADCTL::HORIZ_ORDER;
      }

      if(width == 240 && height == 135) {
        caset[0] = 40;   // 240 cols
        caset[1] = 279;
        raset[0] = 53;   // 135 rows
        raset[1] = 187;
        madctl = MADCTL::COL_ORDER | MADCTL::SWAP_XY | MADCTL::SCAN_ORDER;
      }

      if(width == 135 && height == 240) {
        caset[0] = 52;   // 135 cols
        caset[1] = 186;
        raset[0] = 40;   // 240 rows
        raset[1] = 279;
        madctl = 0;
      }

      if(width == 320 && height == 240) {
        caset[0] = 0;
        caset[1] = 319;
        raset[0] = 0;
        raset[1] = 239;
        madctl = 0x70;
      }

      // Byte swap the 16bit rows/cols values
      caset[0] = __builtin_bswap16(caset[0]);
      caset[1] = __builtin_bswap16(caset[1]);
      raset[0] = __builtin_bswap16(raset[0]);
      raset[1] = __builtin_bswap16(raset[1]);

      command(reg::CASET,     4, (char *)caset);
      command(reg::RASET,     4, (char *)raset);
      command(reg::MADCTL,    1, (char *)&madctl);

      if(bl != PIN_UNUSED) {
        update(); // Send the new buffer to the display to clear any previous content
        sleep_ms(50); // Wait for the update to apply
        set_backlight(255); // Turn backlight on now surprises have passed
      }
    }

    // the dma transfer works but without vsync it's not that useful as you could
    // be updating the framebuffer during transfer...
    //
    // this could be avoided by creating another buffer to draw into and flip
    // buffers (but costs another ~100kb of ram)
    //
    // it's probably not worth it for this particular usecase but will consider it
    // some more...

    // setup spi for 16-bit transfers
    // spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // initialise dma channel for transmitting pixel data to screen
    // dma_channel = dma_claim_unused_channel(true);
    // dma_channel_config config = dma_channel_get_default_config(dma_channel);
    // channel_config_set_transfer_data_size(&config, DMA_SIZE_16);
    // channel_config_set_dreq(&config, spi_get_index(spi) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    // dma_channel_configure(
    //   dma_channel, &config, &spi_get_hw(spi)->dr, frame_buffer, width * height, false);
  }

  spi_inst_t* ST7789::get_spi() const {
    return spi;
  }

  uint ST7789::get_cs() const {
    return cs;
  }

  uint ST7789::get_dc() const {
    return dc;
  }

  uint ST7789::get_sck() const {
    return sck;
  }

  uint ST7789::get_mosi() const {
    return mosi;
  }

  uint ST7789::get_bl() const {
    return bl;
  }

  void ST7789::command(uint8_t command, size_t len, const char *data) {
    //dma_channel_wait_for_finish_blocking(dma_channel);

    gpio_put(cs, 0);

    gpio_put(dc, 0); // command mode
    spi_write_blocking(spi, &command, 1);

    if(data) {
      gpio_put(dc, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(cs, 1);
  }

  void ST7789::update(bool dont_block) {
    command(reg::RAMWR, width * height * sizeof(uint16_t), (const char*)frame_buffer);

    /*if(dma_channel_is_busy(dma_channel) && dont_block) {
      return;
    }

    dma_channel_wait_for_finish_blocking(dma_channel);

    uint8_t r = reg::RAMWR;

    gpio_put(cs, 0);

    gpio_put(dc, 0); // command mode
    spi_write_blocking(spi, &r, 1);

    gpio_put(dc, 1); // data mode

    dma_channel_set_read_addr(dma_channel, frame_buffer, true);*/
  }

  void ST7789::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }

  void ST7789::vsync_callback(gpio_irq_callback_t callback) {
    gpio_set_irq_enabled_with_callback(vsync, GPIO_IRQ_EDGE_RISE, true, callback);
  }

  void ST7789::flip(){
    madctl ^= MADCTL::ROW_ORDER | MADCTL::COL_ORDER;
    command(reg::MADCTL,    1, (char *)&madctl);
  }
}
