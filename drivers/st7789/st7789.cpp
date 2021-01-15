#include "st7789.hpp"

#include "hardware/dma.h"

namespace pimoroni {

  void ST7789::init() {
    spi_init(spi, spi_baud);

    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    // if framesync is enabled then the fs pin is toggled
    // high during vsync
    if(vsync != -1) {
      gpio_set_function(fs, GPIO_FUNC_SIO);
      gpio_set_dir(fs, GPIO_IN);
      gpio_set_pulls(fs, false, true)
    }

    // if a backlight pin is provided then set it up for
    // pwm control
    if(bl != -1) {
      pwm_config cfg = pwm_get_default_config();
      pwm_config_wrap(&cfg, 65535);
      pwm_init(pwm_gpio_to_slice(bl), &cfg, true);
      gpio_set_function(bl, GPIO_FUNC_PWM);
    }

    gpio_set_function(sck,  GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    if(miso != -1) {
      gpio_set_function(miso, GPIO_FUNC_SPI);
    }

    // initialise dma channel for transmitting pixel data to screen
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    //channel_config_set_bswap(&config, true); // byte swap to reverse little endian

    dma_channel_configure(
      dma_channel,
      &config,
      &spi_get_hw(spi)->dr,
      buffer,
      sizeof(uint16_t) * width * height,
      false);
  }

  void ST7789::init_240x240() {
    init();

    width = 240;
    height = 240;
    row_stride = width * sizeof(uint16_t);

    // only initialise the buffer if one hasn't been supplied
    if(!buffer) {
      buffer = malloc(sizeof(uint16_t) * width * height);
    }

    // initialise our standard 240x240 LCD as 16-bit colour
    // with an RGB subpixel element order.
    command(register::SWRESET);

    sleep_ms(150);

    command(register::MADCTL,    1, "\x04");  // row/column addressing order - rgb pixel order
    command(register::TEON,      1, "\x00");  // enable frame sync signal if used
    command(register::COLMOD,    1, "\x05");  // 16 bits per pixel

    command(register::SLPOUT);  // leave sleep mode
    command(register::DISPON);  // turn display on

    sleep_ms(100);

    // set writing "window" to the full display
    command(register::CASET,     4, "\x00\x00\x00\xef");  // 0 .. 239 columns
    command(register::RASET,     4, "\x00\x00\x00\xef");  // 0 .. 239 rows

    // put into write mode
    command(register::RAMWR);
  }

  void ST7789::init_240x135() {
    init();

    width = 240;
    height = 135;
    row_stride = width * sizeof(uint16_t);

    // only initialise the buffer if one hasn't been supplied
    if(!buffer) {
      buffer = malloc(sizeof(uint16_t) * width * height);
    }

    // initialise our standard 240x135 LCD as 16-bit colour
    // with an RGB subpixel element order.
    command(register::SWRESET);

    sleep_ms(150);

    command(register::MADCTL,    1, "\x70");
    command(register::COLMOD,    1, "\x05");

    command(register::SLPOUT);  // leave sleep mode
    command(register::DISPON);  // turn display on

    sleep_ms(100);

    command(register::RASET,     4, "\x00\x35\x00\xbb"); // 53 .. 187 (135 rows)
    command(register::CASET,     4, "\x00\x28\x01\x17"); // 40 .. 279 (240 columns)
  }

  void ST7789::command(uint8_t command, size_t len = 0, const char *data = NULL) {
    gpio_put(pin::CS, 0);

    gpio_put(pin::DC, 0); // command mode
    spi_write_blocking(spi0, &command, 1);

    if(data) {
      gpio_put(pin::DC, 1); // data mode
      spi_write_blocking(spi0, (const uint8_t*)data, len);
    }

    gpio_put(pin::CS, 1);
  }

  void ST7789::update() {
    dma_channel_wait_for_finish_blocking(dma_channel);

    command(register::RAMWR);
    dma_channel_start(dma_channel);
  }

  void ST7789::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65536.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }

  void ST7789::vsync_callback(gpio_irq_callback_t callback) {
    gpio_set_irq_enabled_with_callback(vsync, GPIO_IRQ_EDGE_RISE, true, &callback);
  }
}