#include "st7789.hpp"

#include <cstdlib>
#include <math.h>


namespace pimoroni {

  DMAInterruptHandler *DMAInterruptHandler::dma_interrupt_handlers[2] = {nullptr, nullptr};
  
  uint8_t madctl;
  uint16_t caset[2] = {0, 0};
  uint16_t raset[2] = {0, 0};

  enum MADCTL : uint8_t {
    ROW_ORDER   = 0b10000000,
    COL_ORDER   = 0b01000000,
    SWAP_XY     = 0b00100000,  // AKA "MV"
    SCAN_ORDER  = 0b00010000,
    RGB_BGR     = 0b00001000,
    HORIZ_ORDER = 0b00000100
  };

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


  void ST7789::common_init() {
    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    // if a backlight pin is provided then set it up for
    // pwm control
    if(bl != PIN_UNUSED) {
      pwm_config cfg = pwm_get_default_config();
      pwm_set_wrap(pwm_gpio_to_slice_num(bl), 65535);
      pwm_init(pwm_gpio_to_slice_num(bl), &cfg, true);
      gpio_set_function(bl, GPIO_FUNC_PWM);
      set_backlight(0); // Turn backlight off initially to avoid nasty surprises
    }

    command(reg::SWRESET);

    sleep_ms(150);

    // Common init
    command(reg::TEON);  // enable frame sync signal if used
    command(reg::COLMOD,    1, "\x05");  // 16 bits per pixel

    command(reg::PORCTRL, 5, "\x0c\x0c\x00\x33\x33");
    command(reg::LCMCTRL, 1, "\x2c");
    command(reg::VDVVRHEN, 1, "\x01");
    command(reg::VRHS, 1, "\x12");
    command(reg::VDVS, 1, "\x20");
    command(reg::PWCTRL1, 2, "\xa4\xa1");
    command(reg::FRCTRL2, 1, "\x0f");

    if(width == 240 && height == 240) {
      command(reg::GCTRL, 1, "\x14");
      command(reg::VCOMS, 1, "\x37");
      command(reg::GMCTRP1, 14, "\xD0\x04\x0D\x11\x13\x2B\x3F\x54\x4C\x18\x0D\x0B\x1F\x23");
      command(reg::GMCTRN1, 14, "\xD0\x04\x0C\x11\x13\x2C\x3F\x44\x51\x2F\x1F\x1F\x20\x23");
    }

    if(width == 320 && height == 240) {
      command(reg::GCTRL, 1, "\x35");
      command(reg::VCOMS, 1, "\x1f");
      command(reg::GMCTRP1, 14, "\xD0\x08\x11\x08\x0C\x15\x39\x33\x50\x36\x13\x14\x29\x2D");
      command(reg::GMCTRN1, 14, "\xD0\x08\x10\x08\x06\x06\x39\x44\x51\x0B\x16\x14\x2F\x31");
    }

    if(width == 240 && height == 135) { // Pico Display Pack (1.14" 240x135)
      command(reg::VRHS, 1, "\x00"); // VRH Voltage setting
      command(reg::GCTRL, 1, "\x75"); // VGH and VGL voltages
      command(reg::VCOMS, 1, "\x3D"); // VCOM voltage
      command(0xd6, 1, "\xa1"); // ???
      command(reg::GMCTRP1, 14, "\x70\x04\x08\x09\x09\x05\x2A\x33\x41\x07\x13\x13\x29\x2f");
      command(reg::GMCTRN1, 14, "\x70\x03\x09\x0A\x09\x06\x2B\x34\x41\x07\x12\x14\x28\x2E");
    }

    command(reg::INVON);   // set inversion mode
    command(reg::SLPOUT);  // leave sleep mode
    command(reg::DISPON);  // turn display on

    sleep_ms(100);

    configure_display(rotation);

    if(bl != PIN_UNUSED) {
      //update(); // Send the new buffer to the display to clear any previous content
      sleep_ms(50); // Wait for the update to apply
      set_backlight(255); // Turn backlight on now surprises have passed
    }
  }

  void ST7789::cleanup() {
    if(dma_channel_is_claimed(st_dma_data)) {
      dma_channel_abort(st_dma_data);
      dma_channel_unclaim(st_dma_data);
    }

#if !USE_ASYNC_INTERRUPTS
    if(use_async_dma && dma_channel_is_claimed(st_dma_control_chain)) {
      dma_channel_abort(st_dma_control_chain);
      dma_channel_unclaim(st_dma_control_chain);
    }

		delete[] dma_control_chain_blocks;
#endif 

    if(spi) return; // SPI mode needs no further tear down

    if(pio_sm_is_claimed(parallel_pio, parallel_sm)) {
      pio_sm_set_enabled(parallel_pio, parallel_sm, false);
      pio_sm_drain_tx_fifo(parallel_pio, parallel_sm);
      pio_sm_unclaim(parallel_pio, parallel_sm);
    }
  }

  void ST7789::configure_display(Rotation rotate) {

    bool rotate180 = rotate == ROTATE_180 || rotate == ROTATE_90;

    if(rotate == ROTATE_90 || rotate == ROTATE_270) {
      std::swap(width, height);
    }
		
		// setup full and current regions
		full_screen_region = {0, 0, width, height};
		current_update_region = {0, 0, width, height};

    // 240x240 Square and Round LCD Breakouts
    if(width == 240 && height == 240) {
      int row_offset = round ? 40 : 80;
      int col_offset = 0;
    
      switch(rotate) {
        case ROTATE_90:
          if (!round) row_offset = 0;
          caset[0] = row_offset;
          caset[1] = width + row_offset - 1;
          raset[0] = col_offset;
          raset[1] = width + col_offset - 1;

          madctl = MADCTL::HORIZ_ORDER | MADCTL::COL_ORDER | MADCTL::SWAP_XY;
          break;
        case ROTATE_180:
          caset[0] = col_offset;
          caset[1] = width + col_offset - 1;
          raset[0] = row_offset;
          raset[1] = width + row_offset - 1;

          madctl = MADCTL::HORIZ_ORDER | MADCTL::COL_ORDER | MADCTL::ROW_ORDER;
          break;
        case ROTATE_270:
          caset[0] = row_offset;
          caset[1] = width + row_offset - 1;
          raset[0] = col_offset;
          raset[1] = width + col_offset - 1;

          madctl = MADCTL::ROW_ORDER | MADCTL::SWAP_XY;
          break;
        default: // ROTATE_0 (and for any smart-alec who tries to rotate 45 degrees or something...)
          if (!round) row_offset = 0;
          caset[0] = col_offset;
          caset[1] = width + col_offset - 1;
          raset[0] = row_offset;
          raset[1] = width + row_offset - 1;

          madctl = MADCTL::HORIZ_ORDER;
          break;
      }
    }

    // Pico Display
    if(width == 240 && height == 135) {
      int col_offset = 40;
      int row_offset = 53-rotate180; // something a little weird here, needs offsetting by -1 if rotateing 180!

      caset[0] = col_offset;   
      caset[1] = width + col_offset - 1;
      raset[0] = row_offset;   
      raset[1] = height + row_offset - 1;

      madctl = rotate180 ? MADCTL::ROW_ORDER : MADCTL::COL_ORDER;
      madctl |= MADCTL::SWAP_XY | MADCTL::SCAN_ORDER;
    }

    // Pico Display at 90 degree rotation
    if(width == 135 && height == 240) {
      int col_offset = 52+rotate180; // something a little weird here, needs offsetting by +1 if rotateing 180!
      int row_offset = 40; 

      caset[0] = col_offset;   
      caset[1] = width + col_offset - 1;
      raset[0] = row_offset;   
      raset[1] = height + row_offset - 1;

      madctl = rotate180 ? (MADCTL::COL_ORDER | MADCTL::ROW_ORDER) : 0;
    }

    // Pico Display 2.0
    if(width == 320 && height == 240) {
      caset[0] = 0;
      caset[1] = 319;
      raset[0] = 0;
      raset[1] = 239;
      madctl = (rotate == ROTATE_180 || rotate == ROTATE_90) ? MADCTL::ROW_ORDER : MADCTL::COL_ORDER;
      madctl |= MADCTL::SWAP_XY | MADCTL::SCAN_ORDER;
    }

    // Pico Display 2.0 at 90 degree rotation
    if(width == 240 && height == 320) {
      caset[0] = 0;
      caset[1] = 239;
      raset[0] = 0;
      raset[1] = 319;
      madctl = (rotate == ROTATE_180 || rotate == ROTATE_90) ? (MADCTL::COL_ORDER | MADCTL::ROW_ORDER) : 0;
    }

    // Byte swap the 16bit rows/cols values
		uint16_t scaset[2] = {0, 0};
  	uint16_t sraset[2] = {0, 0};

    scaset[0] = __builtin_bswap16(caset[0]);
    scaset[1] = __builtin_bswap16(caset[1]);
    sraset[0] = __builtin_bswap16(raset[0]);
    sraset[1] = __builtin_bswap16(raset[1]);

    command(reg::CASET,  4, (char *)scaset);
    command(reg::RASET,  4, (char *)sraset);
    command(reg::MADCTL, 1, (char *)&madctl);
  }

  void ST7789::write_blocking_dma(const uint8_t *src, size_t len) {
    while (dma_channel_is_busy(st_dma_data))
      ;
    dma_channel_set_trans_count(st_dma_data, len, false);
    dma_channel_set_read_addr(st_dma_data, src, true);
  }

  void ST7789::write_blocking_parallel(const uint8_t *src, size_t len) {
    write_blocking_dma(src, len);
    dma_channel_wait_for_finish_blocking(st_dma_data);

    // This may cause a race between PIO and the
    // subsequent chipselect deassert for the last pixel
    while(!pio_sm_is_tx_fifo_empty(parallel_pio, parallel_sm))
      ;
  }

  void ST7789::command(uint8_t command, size_t len, const char *data, bool use_async_dma) {
    gpio_put(dc, 0); // command mode

    gpio_put(cs, 0);
    
    if(spi) {
      spi_write_blocking(spi, &command, 1);
    } else {
      write_blocking_parallel(&command, 1);
    }

    if(data) {
      gpio_put(dc, 1); // data mode
      if(spi) {
				if(use_async_dma) {
         	write_blocking_dma((const uint8_t*)data, len);
				}
				else {
        	spi_write_blocking(spi, (const uint8_t*)data, len);
				}
      } else {
        write_blocking_parallel((const uint8_t*)data, len);
      }
    }

		if(!use_async_dma)
			gpio_put(cs, 1);    
  }
  
	void ST7789::partial_update(PicoGraphics *display, Rect region) {
		// check sanity flag
		if(in_dma_update) {
			panic("When use_async_dma is set you must call wait_for_update_to_finish() between updates");
		}
		else {
			in_dma_update = use_async_dma;
		}

    uint8_t cmd = reg::RAMWR;
		if(set_update_region(region)){
			// select and enter command mode
      gpio_put(cs, 0);
      gpio_put(dc, 0); 

			// send RAMWR command
      if(spi) { // SPI Bus
        spi_write_blocking(spi, &cmd, 1);
      } else { // Parallel Bus
        write_blocking_parallel(&cmd, 1);
      }

			// enter data mode
      gpio_put(dc, 1); // data mode

			if(display->pen_type == PicoGraphics::PEN_RGB565) { // Display buffer is screen native
				uint16_t* framePtr = (uint16_t*)display->frame_buffer + region.x + (region.y * width);

				if(use_async_dma) {
#if USE_ASYNC_INTERRUPTS          
					enable_dma_interrupt(true);
          dma_stride.count = region.h;
          dma_stride.data = (uint8_t*)framePtr;
          dma_stride.size = region.w * sizeof(uint16_t);
          dma_stride.width = width * sizeof(uint16_t);
          start_dma_interrupt();
#else
					// Setup chained dma channels
					enable_dma_control_chain(true);
					
					for(int32_t control_idx = 0; control_idx < region.h; control_idx++) {
						dma_control_chain_blocks[control_idx] = { region.w * sizeof(uint16_t), (uint8_t*)framePtr };
						framePtr+=(width);
					}
					dma_control_chain_blocks[region.h] = { 0, 0 };
					start_dma_control_chain();
#endif
				}
				else {
					for(int32_t row = region.y; row < region.y + region.h; row++) {
						spi_write_blocking(spi, (uint8_t*)framePtr, region.w * sizeof(uint16_t));
						framePtr+=(width);
					}
				}
			}
			else
			{
				// use rect_convert to convert to 565
				display->rect_convert(PicoGraphics::PEN_RGB565, region, [this](void *data, size_t length) {
					if (length > 0) {
						write_blocking_dma((const uint8_t*)data, length);
					}
					else {
						dma_channel_wait_for_finish_blocking(st_dma_data);
					}
				});
			}

			// if we are using async dma leave CS alone
			if(!use_async_dma) {
				gpio_put(cs, 1);
			}
		}
	}

  void ST7789::update(PicoGraphics *graphics) {
		// check sanity flag
		if(in_dma_update) {
			panic("When use_async_dma is set you must call wait_for_update_to_finish() between updates");
		}
		else {
			in_dma_update = use_async_dma;
		}

		// set update rect to the full screen
		set_update_region(full_screen_region);

    uint8_t cmd = reg::RAMWR;

    if(graphics->pen_type == PicoGraphics::PEN_RGB565) { // Display buffer is screen native
      command(cmd, width * height * sizeof(uint16_t), (const char*)graphics->frame_buffer, use_async_dma);
    } else {
      gpio_put(dc, 0); // command mode
      gpio_put(cs, 0);
      if(spi) { // SPI Bus
        spi_write_blocking(spi, &cmd, 1);
      } else { // Parallel Bus
        write_blocking_parallel(&cmd, 1);
      }

      gpio_put(dc, 1); // data mode

      graphics->frame_convert(PicoGraphics::PEN_RGB565, [this](void *data, size_t length) {
        if (length > 0) {
          write_blocking_dma((const uint8_t*)data, length);
        }
        else {
          dma_channel_wait_for_finish_blocking(st_dma_data);
        }
      });

      gpio_put(cs, 1);
    }
  }

  void ST7789::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }

	void ST7789::setup_dma_control_chain_if_needed() {
		if(use_async_dma) {
			dma_control_chain_blocks = new DMAControlBlock[height+1];
			st_dma_control_chain = dma_claim_unused_channel(true);

			// config to write 32 bit registers in spi dma
			dma_control_config = dma_channel_get_default_config(st_dma_control_chain);
			channel_config_set_transfer_data_size(&dma_control_config, DMA_SIZE_32);
			channel_config_set_read_increment(&dma_control_config, true);
			channel_config_set_write_increment(&dma_control_config, true);
			channel_config_set_ring(&dma_control_config, true, 3); // wrap at 8 bytes to repeatedly write the count and address registers

			// configure to write to count and address registers from our dma_control_chain_blocks array, write two 32 bit values for len and addr
			dma_channel_configure(st_dma_control_chain,	&dma_control_config, &dma_hw->ch[st_dma_data].al3_transfer_count, &dma_control_chain_blocks[0], 2, false);
		}
	}


	void ST7789::enable_dma_interrupt(bool enable) {
	 	if(use_async_dma) {
 			if(dma_interrupts_is_enabled != enable){
				dma_interrupts_is_enabled = enable;

        if(dma_interrupts_is_enabled) {
          enable_dma_irq(this, st_dma_data, use_dma_interrupt);
        } else
        {
          disable_dma_irq(this, st_dma_data, use_dma_interrupt);
        }
      }
    }
  }

	void ST7789::start_dma_interrupt() {
		if(use_async_dma) {
      if(dma_stride.count && dma_interrupts_is_enabled)
      {
        dma_channel_configure(st_dma_data, &dma_data_config, &spi_get_hw(spi)->dr, dma_stride.data, dma_stride.size, false);

        dma_hw->ints0 = 1u << st_dma_data;
        dma_start_channel_mask(1u << st_dma_data);

        dma_stride.count--;
        dma_stride.data += dma_stride.width;
      }
      else
        in_dma_update = false; // Mark as completed
		}
	}


	void ST7789::enable_dma_control_chain(bool enable) {
		if(use_async_dma) {
			if(dma_control_chain_is_enabled != enable){
				dma_control_chain_is_enabled = enable;
				if(dma_control_chain_is_enabled) {
					// enable dma control chain, chain to control dma and only set irq at end of chain
					channel_config_set_chain_to(&dma_data_config, st_dma_control_chain);
					channel_config_set_irq_quiet(&dma_data_config, true);
				}
				else {
					// disable dma control chain, chain to data dma and set irq at end of transfer
					channel_config_set_chain_to(&dma_data_config, st_dma_data);
					channel_config_set_irq_quiet(&dma_data_config, false);
				}

				// configure the data dma
				if(spi) {
					dma_channel_configure(st_dma_data, &dma_data_config, &spi_get_hw(spi)->dr, NULL, 0, false);
				}
				else {
					dma_channel_configure(st_dma_data, &dma_data_config, &parallel_pio->txf[parallel_sm], NULL, 0, false);
				}

				// configure control chain dma
				dma_channel_configure(st_dma_control_chain,	&dma_control_config, &dma_hw->ch[st_dma_data].al3_transfer_count, &dma_control_chain_blocks[0], 2, false);
			}
		}
	}

	void ST7789::start_dma_control_chain() {
		if(use_async_dma && dma_control_chain_is_enabled) {
   		dma_hw->ints0 = 1u << st_dma_data;
			dma_start_channel_mask(1u << st_dma_control_chain);
		}
	}


	bool ST7789::set_update_region(Rect& update_region){
		// find intersection with display
		update_region = Rect(0,0,width,height).intersection(update_region);

		// check we have a valid region
		bool valid_region = !update_region.empty();

		// if we have a valid region and it is different to the current update rect
		if(valid_region && !update_region.equals(current_update_region)) {
			// offset the rect
			uint16_t scaset[2] = {__builtin_bswap16(caset[0]+update_region.x), __builtin_bswap16(caset[0]+update_region.x+update_region.w-1)};
	  	uint16_t sraset[2] = {__builtin_bswap16(raset[0]+update_region.y), __builtin_bswap16(raset[0]+update_region.y+update_region.h-1)};

			// update display row and column
			command(reg::CASET,  4, (char *)scaset);
			command(reg::RASET,  4, (char *)sraset);

			// update our current region
			current_update_region = update_region;
		}

		return valid_region;
	}
}
