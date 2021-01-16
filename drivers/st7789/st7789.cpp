#include "st7789.hpp"

#include <cstdlib>
#include <math.h>

#include "hardware/dma.h"
#include "hardware/pwm.h"

namespace pimoroni {

  void ST7789::init(bool auto_init_sequence) {
    // configure spi interface and pins
    spi_init(spi, spi_baud);

    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(sck,  GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    if(miso != -1) {
      gpio_set_function(miso, GPIO_FUNC_SPI);
    }

    // if supported by the display then the vsync pin is
    // toggled high during vertical blanking period
    if(vsync != -1) {
      gpio_set_function(vsync, GPIO_FUNC_SIO);
      gpio_set_dir(vsync, GPIO_IN);
      gpio_set_pulls(vsync, false, true);
    }

    // if a backlight pin is provided then set it up for
    // pwm control
    if(bl != -1) {
      pwm_config cfg = pwm_get_default_config();
      pwm_set_wrap(pwm_gpio_to_slice_num(bl), 65535);
      pwm_init(pwm_gpio_to_slice_num(bl), &cfg, true);
      gpio_set_function(bl, GPIO_FUNC_PWM);
    }

    // if auto_init_sequence then send initialisation sequence
    // for our standard displays based on the width and height
    if(auto_init_sequence) {
      command(reg::SWRESET);

      sleep_ms(150);

      if(width == 240 && height == 240) {
        command(reg::MADCTL,    1, "\x04");  // row/column addressing order - rgb pixel order
        command(reg::TEON,      1, "\x00");  // enable frame sync signal if used
        command(reg::COLMOD,    1, "\x05");  // 16 bits per pixel
      }else if(width == 240 && height == 135) {
        command(reg::MADCTL,    1, "\x70");
        command(reg::COLMOD,    1, "\x05");
      }

      command(reg::INVON);   // set inversion mode
      command(reg::SLPOUT);  // leave sleep mode
      command(reg::DISPON);  // turn display on

      sleep_ms(100);

      // setup correct addressing window
      if(width == 240 && height == 240) {
        command(reg::CASET,     4, "\x00\x00\x00\xef");  // 0 .. 239 columns
        command(reg::RASET,     4, "\x00\x00\x00\xef");  // 0 .. 239 rows
      }else if(width == 240 && height == 135) {
        command(reg::RASET,     4, "\x00\x35\x00\xbb"); // 53 .. 187 (135 rows)
        command(reg::CASET,     4, "\x00\x28\x01\x17"); // 40 .. 279 (240 columns)
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
    ST7789::command(reg::RAMWR, width * height * sizeof(uint16_t), (const char*)frame_buffer);

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
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65536.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }

  void ST7789::vsync_callback(gpio_irq_callback_t callback) {
    gpio_set_irq_enabled_with_callback(vsync, GPIO_IRQ_EDGE_RISE, true, callback);
  }
}