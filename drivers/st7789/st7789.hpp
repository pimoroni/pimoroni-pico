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
#include "st7789_parallel.pio.h"
#endif

#include <algorithm>


namespace pimoroni {

  class ST7789 : public DisplayDriver {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;
  
  public:
    bool round;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint wr_sck;
    uint rd_sck = PIN_UNUSED;
    uint d0;
    uint bl;
    uint vsync  = PIN_UNUSED; // only available on some products
    uint parallel_sm;
    PIO parallel_pio;
    uint parallel_offset;
    uint st_dma;


    // The ST7789 requires 16 ns between SPI rising edges.
    // 16 ns = 62,500,000 Hz
    static const uint32_t SPI_BAUD = 62'500'000;


  public:
    // Parallel init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, ParallelPins pins) :
      DisplayDriver(width, height, rotation),
      spi(nullptr), round(false),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.wr_sck), rd_sck(pins.rd_sck), d0(pins.d0), bl(pins.bl) {

      parallel_pio = pio1;
      parallel_sm = pio_claim_unused_sm(parallel_pio, true);
      parallel_offset = pio_add_program(parallel_pio, &st7789_parallel_program);
  
      //gpio_init(wr_sck);
      //gpio_set_dir(wr_sck, GPIO_OUT);
      //gpio_set_function(wr_sck, GPIO_FUNC_SIO);
      pio_gpio_init(parallel_pio, wr_sck);

      gpio_set_function(rd_sck,  GPIO_FUNC_SIO);
      gpio_set_dir(rd_sck, GPIO_OUT);

      for(auto i = 0u; i < 8; i++) {
        //gpio_set_function(d0 + i, GPIO_FUNC_SIO);
        //gpio_set_dir(d0 + i, GPIO_OUT);
        //gpio_init(d0 + 0); gpio_set_dir(d0 + i, GPIO_OUT);
        pio_gpio_init(parallel_pio, d0 + i);
      }

      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, d0, 8, true);
      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, wr_sck, 1, true);

      pio_sm_config c = st7789_parallel_program_get_default_config(parallel_offset);

      sm_config_set_out_pins(&c, d0, 8);
      sm_config_set_sideset_pins(&c, wr_sck);
      sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
      sm_config_set_out_shift(&c, false, true, 8);
      
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
  
      gpio_put(rd_sck, 1);

      common_init();
    }

    // Serial init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, SPIPins pins) :
      DisplayDriver(width, height, rotation),
      spi(pins.spi), round(round),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.sck), d0(pins.mosi), bl(pins.bl) {

      // configure spi interface and pins
      spi_init(spi, SPI_BAUD);

      gpio_set_function(wr_sck, GPIO_FUNC_SPI);
      gpio_set_function(d0, GPIO_FUNC_SPI);

      st_dma = dma_claim_unused_channel(true);
      dma_channel_config config = dma_channel_get_default_config(st_dma);
      channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
      channel_config_set_bswap(&config, false);
      channel_config_set_dreq(&config, spi_get_dreq(spi, true));
      dma_channel_configure(st_dma, &config, &spi_get_hw(spi)->dr, NULL, 0, false);
  
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
