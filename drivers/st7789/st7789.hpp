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

// set USE_ASYNC_INTERRUPTS to 1 to use interrupts rather than control chain for python.
#define USE_ASYNC_INTERRUPTS 0  

namespace pimoroni {
  
  class DMAInterruptHandler {

  protected:
    static DMAInterruptHandler *dma_interrupt_handlers[2];

    static void __isr dma_handler_irq() {
      dma_interrupt_handlers[0]->handle_dma_irq();
    }

    static void enable_dma_irq(DMAInterruptHandler *pObject, uint channel, uint dma_irq)
    {
      dma_channel_set_irq0_enabled(channel, true);
      irq_set_exclusive_handler(dma_irq, dma_handler_irq);
      dma_interrupt_handlers[dma_irq-DMA_IRQ_0] = pObject;
      irq_set_enabled(dma_irq, true);
    }

    static void disable_dma_irq(DMAInterruptHandler *pObject, uint channel, uint dma_irq)
    {
      dma_channel_set_irq0_enabled(channel, false);
      irq_remove_handler(dma_irq, dma_handler_irq);
      dma_interrupt_handlers[dma_irq-DMA_IRQ_0] = nullptr;
      irq_set_enabled(dma_irq, false);
    }

    bool is_channel_interrupt_set(uint channel)
    {
      return dma_hw->ints0 & (1u << channel);
    }

    void clear_dma_channel_interrupt(uint channel)
    {
      dma_hw->ints0 = (1u << channel); // clear irq flag
    }

    virtual void handle_dma_irq(void) = 0;
  };

  class ST7789 : public DisplayDriver, public DMAInterruptHandler {
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
    uint st_dma_data;


    // The ST7789 requires 16 ns between SPI rising edges.
    // 16 ns = 62,500,000 Hz
    // 2350 doesn't support 62,500,000 so use 75,000,000 seems to work.
#if !PICO_RP2350
    static const uint32_t SPI_BAUD = 62'500'000;
#else
    static const uint32_t SPI_BAUD = 75'000'000;
#endif

		// current update rect and full screen rect
		Rect								full_screen_region;
		Rect								current_update_region;

	  // dma control blocks used for async partial updates
		struct DMAControlBlock
		{
			uint32_t len; 
			uint8_t* data;
		};

    struct DMAStride
		{
      uint32_t size = 0;
      uint32_t count = 0;
      uint32_t width = 0;
			uint8_t* data = nullptr;
		};

		uint 								st_dma_control_chain;
    DMAStride           dma_stride;
		DMAControlBlock* 		dma_control_chain_blocks = nullptr;
		dma_channel_config 	dma_data_config;
		dma_channel_config  dma_control_config;
		bool 								use_async_dma = false;
    u_char              use_dma_interrupt;
		bool								dma_control_chain_is_enabled = false;
    bool								dma_interrupts_is_enabled = false;

		// sanity flag for dma updates
		bool								in_dma_update = false;


  public:
    // Parallel init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, ParallelPins pins, bool use_async_dma = false, uint use_dma_interrupt = DMA_IRQ_0) :
      DisplayDriver(width, height, rotation),
      spi(nullptr), round(false),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.wr_sck), rd_sck(pins.rd_sck), d0(pins.d0), bl(pins.bl), use_async_dma(use_async_dma), use_dma_interrupt(use_dma_interrupt) {

      parallel_pio = pio1;
      pio_set_gpio_base(parallel_pio, d0 + 8 >= 32 ? 16 : 0);
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


      st_dma_data = dma_claim_unused_channel(true);
      dma_data_config = dma_channel_get_default_config(st_dma_data);
      channel_config_set_transfer_data_size(&dma_data_config, DMA_SIZE_8);
      channel_config_set_bswap(&dma_data_config, false);
      channel_config_set_dreq(&dma_data_config, pio_get_dreq(parallel_pio, parallel_sm, true));
      dma_channel_configure(st_dma_data, &dma_data_config, &parallel_pio->txf[parallel_sm], NULL, 0, false);
  
      gpio_put(rd_sck, 1);

#if !USE_ASYNC_INTERRUPTS
			setup_dma_control_chain_if_needed();
#endif

      common_init();
    }

    // Serial init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, SPIPins pins, bool use_async_dma = false, uint use_dma_interrupt = DMA_IRQ_0) :
      DisplayDriver(width, height, rotation),
      spi(pins.spi), round(round),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.sck), d0(pins.mosi), bl(pins.bl), use_async_dma(use_async_dma), use_dma_interrupt(use_dma_interrupt)  {

      // configure spi interface and pins
      spi_init(spi, SPI_BAUD);

      gpio_set_function(wr_sck, GPIO_FUNC_SPI);
      gpio_set_function(d0, GPIO_FUNC_SPI);

      st_dma_data = dma_claim_unused_channel(true);
      dma_data_config = dma_channel_get_default_config(st_dma_data);
      channel_config_set_transfer_data_size(&dma_data_config, DMA_SIZE_8);
      channel_config_set_bswap(&dma_data_config, false);
      channel_config_set_dreq(&dma_data_config, spi_get_dreq(spi, true));
      dma_channel_configure(st_dma_data, &dma_data_config, &spi_get_hw(spi)->dr, NULL, 0, false);

#if !USE_ASYNC_INTERRUPTS
			setup_dma_control_chain_if_needed();
#endif
      common_init();
    }
	
		virtual ~ST7789()
		{
			cleanup();
		}

    void cleanup() override;
    void update(PicoGraphics *graphics) override;
		void partial_update(PicoGraphics *display, Rect region) override;
    void set_backlight(uint8_t brightness) override;

    
		bool is_busy() override
		{
#if USE_ASYNC_INTERRUPTS
      if(use_async_dma && dma_interrupts_is_enabled) {
        return in_dma_update;
      }
#else
			if(use_async_dma && dma_control_chain_is_enabled) {
				return !(dma_hw->intr & 1u << st_dma_data);
			}
#endif
			else {
				return dma_channel_is_busy(st_dma_data);
			}
		}

		void wait_for_update_to_finish()
		{
#if USE_ASYNC_INTERRUPTS
			if(use_async_dma && dma_interrupts_is_enabled) {
        while(in_dma_update) {
        	tight_loop_contents();
        }

        enable_dma_interrupt(false);
      }
#else
			if(use_async_dma && dma_control_chain_is_enabled) {
				while (!(dma_hw->intr & 1u << st_dma_data)) {
        	tight_loop_contents();
				}

				// disable control chain dma
				enable_dma_control_chain(false);
			}
#endif
			else {
				dma_channel_wait_for_finish_blocking(st_dma_data);
			}

			// deselect 
			gpio_put(cs, 1);

			// set sanity flag
			in_dma_update = false;
		}

  private:
    void common_init();
    void configure_display(Rotation rotate);
    void write_blocking_dma(const uint8_t *src, size_t len);
    void write_blocking_parallel(const uint8_t *src, size_t len);
    void command(uint8_t command, size_t len = 0, const char *data = NULL, bool use_async_dma = false);
		void setup_dma_control_chain_if_needed();
		void enable_dma_control_chain(bool enable);
		void start_dma_control_chain();
		void enable_dma_interrupt(bool enable);
		void start_dma_interrupt();
		bool set_update_region(Rect& update_rect);

    void handle_dma_irq(void) override
    {
      if(is_channel_interrupt_set(st_dma_data))
      {
        clear_dma_channel_interrupt(st_dma_data);
        start_dma_interrupt();
      }
    }
  };
}
