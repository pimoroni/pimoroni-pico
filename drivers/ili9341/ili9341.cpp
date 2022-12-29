#include "ili9341.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {

  enum MadCtl : uint8_t {
    MY  = 0x80, ///< Bottom to top
    MX  = 0x40, ///< Right to left
    MV  = 0x20, ///< Reverse Mode
    ML  = 0x10, ///< LCD refresh Bottom to top
    RGB = 0x00, ///< Red-Green-Blue pixel order
    BGR = 0x08, ///< Blue-Green-Red pixel order
    MH  = 0x04 , ///< LCD refresh right to left
  };


  enum reg {
    NOP 				= 0x00,     ///< No-op register
    SWRESET 		= 0x01, ///< Software reset register
    RDDID  			= 0x04, ///< Read display identification information
    RDDST  			= 0x09, ///< Read Display Status		
    SLPIN  			= 0x10, ///< Enter Sleep Mode
    SLPOUT 			= 0x11, ///< Sleep Out
    PTLON  			= 0x12, ///< Partial Mode ON
    NORON  			= 0x13, ///< Normal Display Mode ON		
    RDMODE 			= 0x0A, ///< Read Display Power Mode
    RDMADCTL 		= 0x0B, ///< Read Display MADCTL
    RDPIXFMT 		= 0x0C, ///< Read Display Pixel Format
    RDIMGFMT 		= 0x0D, ///< Read Display Image Format
    RDSELFDIAG 	= 0x0F, ///< Read Display Self-Diagnostic Result		
    INVOFF 			= 0x20, ///< Display Inversion OFF
    INVON 			= 0x21, ///< Display Inversion ON
    GAMMASET 		= 0x26, ///< Gamma Set
    DISPOFF  		= 0x28, ///< Display OFF
    DISPON   		= 0x29, ///< Display ON		
    CASET 			= 0x2A, ///< Column Address Set
    PASET 			= 0x2B, ///< Page Address Set
    RAMWR 			= 0x2C, ///< Memory Write
    RAMRD 			= 0x2E, ///< Memory Read		
    PTLAR 			= 0x30, ///< Partial Area
    VSCRDEF  		= 0x33, ///< Vertical Scrolling Definition
    MADCTL   		= 0x36, ///< Memory Access Control
    VSCRSADD 		= 0x37, ///< Vertical Scrolling Start Address
    PIXFMT   		= 0x3A, ///< COLMOD: Pixel Format Set		
    WRDISBV 		= 0x51, ///< White Display Brightness
    FRMCTR1  		= 0xB1, ///< Frame Rate Control (In Normal Mode/Full Colors)
    FRMCTR2 		= 0xB2, ///< Frame Rate Control (In Idle Mode/8 colors)
    FRMCTR3 		= 0xB3, ///< Frame Rate control (In Partial Mode/Full Colors)
    INVCTR  		= 0xB4, ///< Display Inversion Control
    DFUNCTR 		= 0xB6, ///< Display Function Control		
    PWCTR1 			= 0xC0, ///< Power Control 1
    PWCTR2 			= 0xC1, ///< Power Control 2
    PWCTR3 			= 0xC2, ///< Power Control 3
    PWCTR4 			= 0xC3, ///< Power Control 4
    PWCTR5 			= 0xC4, ///< Power Control 5
    VMCTR1 			= 0xC5, ///< VCOM Control 1
    VMCTR2 			= 0xC7, ///< VCOM Control 2		
    PWCTRA 			= 0xCB, ///< Power Control A
    PWCTRB 			= 0xCF, ///< Power Control B
    RDID1 			= 0xDA, ///< Read ID 1
    RDID2 			= 0xDB, ///< Read ID 2
    RDID3 			= 0xDC, ///< Read ID 3
    RDID4 			= 0xDD, ///< Read ID 4		
    GMCTRP1 		= 0xE0, ///< Positive Gamma Correction
    GMCTRN1 		= 0xE1, ///< Negative Gamma Correction
    DTCA 	 			= 0xE8, ///< Driver Timing Control A
    DTCB 				= 0xEA, ///< Driver Timing Control B
    POWOSC 			= 0xED, ///< Power On Sequence Control
    E3G					= 0xF2, ///< Enable 3G
    PRC					= 0xF7  ///< Pump Ratio Control
  };

  void ILI9341::common_init() {
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

    // if reset pin is provided then set it up
    if(rst != PIN_UNUSED)
    {
      gpio_set_function(rst, GPIO_FUNC_SIO);
      gpio_set_dir(rst, GPIO_OUT);
    }

    reset();

    sleep_ms(150);

    // Common init
    //		0xEF, 3, 0x03, 0x80, 0x02, // not sure this is needed and it is undocumented.
    command(reg::DISPOFF);
    command(reg::PWCTRB, 	 3,  "\x00\xC1\x30");
    command(reg::POWOSC, 	 3,  "\x64\x03\x12\x81");
    command(reg::DTCA, 		 3,  "\x85\x00\x78");
    command(reg::PWCTRA,   5,  "\x39\x2C\x00\x34\x02");
    command(reg::PRC, 		 1,  "\x20");
    command(reg::DTCB, 		 2,  "\x00\x00");
    command(reg::PWCTR1,   1,  "\x23");	   				// Power control VRH[5:0]
    command(reg::PWCTR2, 	 1,  "\x10");	   				// Power control SAP[2:0];BT[3:0]
    command(reg::VMCTR1,   2,  "\x3e\x28"); 			// VCM control
    command(reg::VMCTR2,   1,  "\x86");	   				// VCM control2
    command(reg::MADCTL,   1,  "\x48");	   				// Memory Access Control
    command(reg::PIXFMT,   1,  "\x55");
    command(reg::FRMCTR1,  2,  "\x00\x13"); 			//refresh rate is here
    command(reg::DFUNCTR,  3,  "\x08\x82\x27");		// Display Function Control
    command(reg::E3G, 		 1,  "\x00");						// 3Gamma Function Disable
    command(reg::GAMMASET, 1,  "\x01");							// Gamma curve selected
    command(reg::GMCTRP1,  15, "\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00");
    command(reg::GMCTRN1,  15, "\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F");
    command(reg::PASET, 	 4,  "\x00\x00\x01\x3f");
    command(reg::CASET, 	 4,  "\x00\x00\x00\xef");

    command(reg::SLPOUT); // Exit Sleep
    sleep_ms(150);

    command(reg::DISPON); // Display on
    sleep_ms(150);


    configure_display(rotation);
  }

  void ILI9341::cleanup() {
    delete[] dma_control_chain_blocks;

    if(dma_channel_is_claimed(st_dma_data)) {
      dma_channel_abort(st_dma_data);
      dma_channel_unclaim(st_dma_data);
    }

    if(use_async_dma && dma_channel_is_claimed(st_dma_control_chain)) {
      dma_channel_abort(st_dma_control_chain);
      dma_channel_unclaim(st_dma_control_chain);
    }
  }


  void ILI9341::configure_display(Rotation rotate) {

    if(rotate == ROTATE_90 || rotate == ROTATE_270) {
      std::swap(width, height);
    }

    // setup full and current regions
    full_screen_region = {0, 0, width, height};
    current_update_region = {0, 0, 0, 0};

    uint8_t madctl;

    switch (rotation)
    {
      case ROTATE_0:
        madctl = (MadCtl::MX | MadCtl::BGR);
      break;

      case ROTATE_90:
        madctl = (MadCtl::MV | MadCtl::BGR);
      break;

      case ROTATE_180:
        madctl = (MadCtl::MY | MadCtl::BGR);
      break;

      case ROTATE_270:
        madctl = (MadCtl::MX | MadCtl::MY | MadCtl::MV | MadCtl::BGR);
      break;
    }

    command(reg::MADCTL, 1, (const char*)&madctl);

    // default to full screen
    set_update_region(full_screen_region);
  }

  void ILI9341::write_blocking_dma(const uint8_t *src, size_t len) {
    while (dma_channel_is_busy(st_dma_data))
      ;
    dma_channel_set_trans_count(st_dma_data, len, false);
    dma_channel_set_read_addr(st_dma_data, src, true);
  }

  void ILI9341::command(uint8_t command, size_t len, const char *data, bool bDataDma) {
    gpio_put(dc, 0); // command mode

    gpio_put(cs, 0);
    spi_set_format(spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_write_blocking(spi, &command, 1);
    
    if(data) {
      gpio_put(dc, 1); // data mode
  
      if(bDataDma) {
        write_blocking_dma((const uint8_t*)data, len);
      }
      else {
        spi_write_blocking(spi, (const uint8_t*)data, len);
      }
    }

    if(!bDataDma)
      gpio_put(cs, 1);    
  }
  
  void ILI9341::partial_update(PicoGraphics *display, Rect region) {
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
      spi_write_blocking(spi, &cmd, 1);

      // enter data mode
      gpio_put(dc, 1); // data mode

      if(display->pen_type == PicoGraphics::PEN_RGB565) { // Display buffer is screen native
        uint16_t* framePtr = (uint16_t*)display->frame_buffer + region.x + (region.y * width);

        if(use_async_dma) {
          // TODO for dma the pico doesn't support dma stride so we need chained dma channels
          // simple first test wait for dma to complete
          enable_dma_control(true);
          
          for(int32_t control_idx = 0; control_idx < region.h; control_idx++) {
            dma_control_chain_blocks[control_idx] = { region.w * sizeof(uint16_t), (uint8_t*)framePtr };
            framePtr+=(width);
          }
          dma_control_chain_blocks[region.h] = { 0, 0 };
          start_dma_control();

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

  void ILI9341::update(PicoGraphics *graphics) {
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

      spi_write_blocking(spi, &cmd, 1);

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

  void ILI9341::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }

  void ILI9341::setup_dma_control_if_needed() {
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

  void ILI9341::enable_dma_control(bool enable) {
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
        dma_channel_configure(st_dma_data, &dma_data_config, &spi_get_hw(spi)->dr, NULL, 0, false);
        
        // configure control chain dma
        dma_channel_configure(st_dma_control_chain,	&dma_control_config, &dma_hw->ch[st_dma_data].al3_transfer_count, &dma_control_chain_blocks[0], 2, false);
      }
    }
  }

  void ILI9341::start_dma_control() {
    if(use_async_dma && dma_control_chain_is_enabled) {
       dma_hw->ints0 = 1u << st_dma_data;
      dma_start_channel_mask(1u << st_dma_control_chain);
    }
  }

  bool ILI9341::set_update_region(Rect& update_region){
    // find intersection with display
    update_region = Rect(0,0,width,height).intersection(update_region);

    // check we have a valid region
    bool valid_region = !update_region.empty();

    // if we have a valid region and it is different to the current update rect
    if(valid_region && !update_region.equals(current_update_region)) {
      set_addr_window(update_region.x, update_region.y, update_region.w, update_region.h);

      // update our current region
      current_update_region = update_region;
    }

    return valid_region;
  }

  void ILI9341::set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)	{
    uint32_t xa = ((uint32_t) x << 16) | (x + w - 1);
    uint32_t ya = ((uint32_t) y << 16) | (y + h - 1);

    xa = __builtin_bswap32(xa);
    ya = __builtin_bswap32(ya);

    command(reg::CASET, sizeof(xa), (const char*)(&xa));
    command(reg::PASET, sizeof(ya), (const char*)(&ya));
    command(reg::RAMWR);
  }

  void ILI9341::reset()	{
    if (rst != PIN_UNUSED)
    {
      gpio_put(rst, 0);
      sleep_ms(5);
      gpio_put(rst, 1);
      sleep_ms(150);
    }
    else
    {	
      command(reg::SWRESET); // Engage software reset
      sleep_ms(150);
    }
  }
}
