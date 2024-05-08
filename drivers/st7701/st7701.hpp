#pragma once

#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"


#ifndef NO_QSTR
#include "st7701_parallel.pio.h"
#endif

#include <algorithm>


namespace pimoroni {

  class ST7701 : public DisplayDriver {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:

    // interface pins with our standard defaults where appropriate
    uint spi_cs;
    uint spi_sck;
    uint spi_dat;
    uint lcd_bl;
    uint parallel_sm;
    PIO parallel_pio;
    uint parallel_offset;
    uint st_dma;

    uint d0 = 1; // First pin of 18-bit parallel interface
    uint hsync  = 19;
    uint vsync  = 20;
    uint lcd_de = 21;
    uint lcd_dot_clk = 22;

    static const uint32_t SPI_BAUD = 8'000'000;

  public:
    // Parallel init
    ST7701(uint16_t width, uint16_t height, Rotation rotation, SPIPins control_pins,
      uint d0=1, uint hsync=19, uint vsync=20, uint lcd_de = 21, uint lcd_dot_clk = 22) :
      DisplayDriver(width, height, rotation),
      spi(control_pins.spi),
      spi_cs(control_pins.cs), spi_sck(control_pins.sck), spi_dat(control_pins.mosi), lcd_bl(control_pins.bl),
      d0(d0), hsync(hsync), vsync(vsync), lcd_de(lcd_de), lcd_dot_clk(lcd_dot_clk) {

      parallel_pio = pio1;
      parallel_sm = pio_claim_unused_sm(parallel_pio, true);
      parallel_offset = pio_add_program(parallel_pio, &st7701_parallel_program);

      spi_init(spi, SPI_BAUD);
      gpio_set_function(spi_cs, GPIO_FUNC_SIO);
      gpio_set_dir(spi_cs, GPIO_OUT);
      gpio_set_function(spi_dat, GPIO_FUNC_SPI);
      gpio_set_function(spi_sck, GPIO_FUNC_SPI);

      // ST7701 3-line Serial Interface
      // 9th bit = D/CX
      // low = command
      // high = data
      spi_set_format(spi, 9, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  
      //gpio_init(wr_sck);
      //gpio_set_dir(wr_sck, GPIO_OUT);
      //gpio_set_function(wr_sck, GPIO_FUNC_SIO);
      pio_gpio_init(parallel_pio, hsync);
      pio_gpio_init(parallel_pio, vsync);
      pio_gpio_init(parallel_pio, lcd_de);
      pio_gpio_init(parallel_pio, lcd_dot_clk);

      for(auto i = 0u; i < 18; i++) {
        //gpio_set_function(d0 + i, GPIO_FUNC_SIO);
        //gpio_set_dir(d0 + i, GPIO_OUT);
        //gpio_init(d0 + 0); gpio_set_dir(d0 + i, GPIO_OUT);
        pio_gpio_init(parallel_pio, d0 + i);
      }

      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, d0, 18, true);
      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, hsync, 4, true);

      pio_sm_config c = st7701_parallel_program_get_default_config(parallel_offset);

      sm_config_set_out_pins(&c, d0, 18);
      //sm_config_set_sideset_pins(&c, wr_sck);
      sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
      sm_config_set_out_shift(&c, false, true, 18);
      
      // Determine clock divider
      constexpr uint32_t max_pio_clk = 32 * MHZ;
      const uint32_t sys_clk_hz = clock_get_hz(clk_sys);
      const uint32_t clk_div = (sys_clk_hz + max_pio_clk - 1) / max_pio_clk;
      sm_config_set_clkdiv(&c, clk_div);
      
      pio_sm_init(parallel_pio, parallel_sm, parallel_offset, &c);
      pio_sm_set_enabled(parallel_pio, parallel_sm, true);


      st_dma = dma_claim_unused_channel(true);
      dma_channel_config config = dma_channel_get_default_config(st_dma);
      channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
      channel_config_set_bswap(&config, false);
      channel_config_set_dreq(&config, pio_get_dreq(parallel_pio, parallel_sm, true));
      dma_channel_configure(st_dma, &config, &parallel_pio->txf[parallel_sm], NULL, 0, false);

      common_init();
    }

    void cleanup() override;
    void update(PicoGraphics *graphics) override;
    void set_backlight(uint8_t brightness) override;

  private:
    void common_init();
    void configure_display(Rotation rotate);
    void write_blocking_dma(const uint8_t *src, size_t len);
    void write_blocking_parallel(const uint8_t *src, size_t len);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
