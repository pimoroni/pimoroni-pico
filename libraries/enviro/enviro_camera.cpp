#include "enviro_camera.hpp"
#include "ov2640.pio.h"
#include "ov2640_config.hpp"

namespace enviro {

  EnviroCamera::EnviroCamera() : EnviroBase() {}

  bool EnviroCamera::init() {
    EnviroBase::init();

    // generate XCLK (wraps at 6 so 125mhz / 6 = 20.83MHz)
    gpio_set_function(EnviroCamera::XCLK, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(EnviroCamera::XCLK);
    pwm_set_wrap(slice_num, 5); // starts at 0, so 6 steps in total 0..5
    pwm_set_gpio_level(EnviroCamera::XCLK, 3); // 50% duty cycle
    pwm_set_enabled(slice_num, true);

    // write initial camera configuration
    regs_write(ov2640_vga);
    regs_write(ov2640_uxga_cif);

    // ??
    reg_write(0xff, 0x00);
    reg_write(0xDA, (reg_read(0xDA) & 0xC) | 0x8);

    uint offset = pio_add_program(pio, &ov2640_program);
    ov2640_program_init(pio, pio_sm, offset);

    // gpio_set_function(VSYNC, GPIO_FUNC_SIO);
    // gpio_set_dir(VSYNC, GPIO_IN);
    // gpio_pull_down(VSYNC);

    dma_channel_config c = dma_channel_get_default_config(0);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);

    dma_channel_configure(0, &c, nullptr, &pio->rxf[pio_sm], 0, false);

    gpio_set_function(EXTERNAL_INTERRUPT_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(EXTERNAL_INTERRUPT_PIN, GPIO_IN);

    if(gpio_get(EXTERNAL_INTERRUPT_PIN))
      woken_up_by = REASON_CAMERA_TRIGGER;

    return true;
  }

  bool EnviroCamera::capture(uint8_t *fb) {
    static uint8_t scanline_buffer_1[1600 * 2];
    static uint8_t scanline_buffer_2[1600 * 2];

    uint32_t scanline_bytes = width * 2;

    uint8_t *process_scanline_buffer = scanline_buffer_1;
    uint8_t *capture_scanline_buffer = scanline_buffer_2;

    // wait for start of next vsync (next falling edge)
    while( gpio_get(VSYNC)) {} // while vsync is high
    while(!gpio_get(VSYNC)) {} // while vsync is low
    
    for(int scanline = -1; scanline < height; scanline++) {
      // take a copy of the scanline for processing
      process_scanline_buffer = capture_scanline_buffer;
      capture_scanline_buffer = process_scanline_buffer == scanline_buffer_1 ? scanline_buffer_2 : scanline_buffer_1;

      // wait for dma channel to be free
      while(dma_channel_is_busy(0)) {}

      // capture the next scanline into scanline_buffer
      dma_channel_transfer_to_buffer_now(0, capture_scanline_buffer, scanline_bytes);

      if(scanline >= 0) {
        // downtime where we can do some processing
        memcpy(fb + (scanline * scanline_bytes), process_scanline_buffer, scanline_bytes);
      }
    }

    return true; 
  }

  void EnviroCamera::reg_write(uint8_t reg, uint8_t value) {
    uint8_t data[] = {reg, value};
    i2c_write_blocking(i2c.get_i2c(), 0x30, data, sizeof(data), false);
  }

  uint8_t EnviroCamera::reg_read(uint8_t reg) {
    i2c_write_blocking(i2c.get_i2c(), 0x30, &reg, 1, false);
    uint8_t value;
    i2c_read_blocking(i2c.get_i2c(), 0x30, &value, 1, false);
    return value;
  }

  void EnviroCamera::regs_write(const uint8_t (*regs)[2]) {
    while (1) {
      uint8_t reg = (*regs)[0];
      uint8_t value = (*regs)[1];
      if (reg == 0x00 && value == 0x00) {break;}
      reg_write(reg, value);
      regs++;
    }
  }

};