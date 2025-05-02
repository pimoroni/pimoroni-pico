#include "ssd1680.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {

  enum reg {
    DOC      = 0x01,  // Set Gate Driver Output
    GDVC     = 0x03,  // Gate level voltage control
    SDVC     = 0x04,  // source driving voltage control
    BTST     = 0x0C,  // booster soft start
    DSM      = 0x10,  // deep sleep mode
    SWR      = 0x12,  // SW RESET
    ADUS     = 0x20,  // activate display update
    DUC1     = 0x21,  // display update control 1
    DUC2     = 0x22,  // display update control 2
    WRAM_BW  = 0x24,  // Write RAM (Black White)
    WRAM_R   = 0x26,  // Write RAM (Red) or gray in our case.
    READ_RAM = 0x27,
    WVCOM    = 0x2C,  // write VCOM reg
    WLR      = 0x32,  // write LUT register
    BWCTRL   = 0x3C,  // border waveform control
    EOPT     = 0x3F,  // option for LUT end
    READ_RAM_OPT = 0x41, // read ram option
    SRX      = 0x44,  // set RAM x start/end pos
    SRY      = 0x45,  // set RAM y start/end pos
    SRXC     = 0x4E,  // set RAM x counter
    SRYC     = 0x4F,  // set RAM y counter
  };


  bool SSD1680::is_busy() {
    if(BUSY == PIN_UNUSED) return false;
    return gpio_get(BUSY);
  }

  void SSD1680::busy_wait() {
    while(is_busy()) {
      tight_loop_contents();
    }
  }

  void SSD1680::reset() {
    if(RESET == PIN_UNUSED) return;
    gpio_put(RESET, 0); sleep_ms(10);
    gpio_put(RESET, 1); sleep_ms(10);
    busy_wait();
  }

  void SSD1680::default_luts() {
    command(WLR, {											
      0x40,	0x68,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L0				
      0xA0,	0x65,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L1				
      0xA8,	0x65,	0x40,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L2				
      0xAA,	0x65,	0x50,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L3 				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	//VS L4 				
      0x02,	0x00,	0x00,	0x05,	0x0A,	0x00,	0x00,						//TP, SR, RP of Group0				
      0x19,	0x19,	0x00,	0x02,	0x00,	0x00,	0x00,						//TP, SR, RP of Group1				
      0x05,	0x0A,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group2				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group3				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group4				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group5				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group6				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group7				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group8				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group9				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group10				
      0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,						//TP, SR, RP of Group11				
      0x44,	0x42,	0x22,	0x22,	0x23,	0x32,	0x00,	0x00,	0x00,				//FR, XON									
      });

    command(EOPT, {0x22});
    command(GDVC, {0x17});
    command(SDVC, {0x41,	0xAE,	0x32});
    command(WVCOM, {0x28});
  
    busy_wait();

  }

  void SSD1680::init() {
    // configure spi interface and pins
    spi_init(spi, 12'000'000);

    gpio_set_function(DC, GPIO_FUNC_SIO);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_set_function(CS, GPIO_FUNC_SIO);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);

    gpio_set_function(RESET, GPIO_FUNC_SIO);
    gpio_set_dir(RESET, GPIO_OUT);
    gpio_put(RESET, 1);

    gpio_set_function(BUSY, GPIO_FUNC_SIO);
    gpio_set_dir(BUSY, GPIO_IN);
    gpio_set_pulls(BUSY, false, true);

    gpio_set_function(SCK,  GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);

    setup();
  };

  void SSD1680::setup() {
    reset();

    command(SWR);
    busy_wait();

    command(DOC, {Y_START_L, Y_START_H, 0x00});
    command(SRX, {X_START, X_END});
    command(SRY, {Y_START_L, Y_START_H, Y_END_L, Y_END_H});
    command(BWCTRL, {0x00});

    busy_wait();
    
  }

  void SSD1680::power_off() {
    //command(POF);
  }

  void SSD1680::read(uint8_t reg, size_t len, uint8_t *data) {
    gpio_put(CS, 0);

    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    if(len > 0) {
      gpio_put(DC, 1); // data mode
      gpio_set_function(SCK,  GPIO_FUNC_SIO);
      gpio_set_dir(SCK, GPIO_OUT);
      gpio_set_function(MOSI, GPIO_FUNC_SIO);
      gpio_set_dir(MOSI, GPIO_IN);
      for(auto i = 0u; i < len; i++) {
        int byte = i / 8;
        int bit = i % 8;
        gpio_put(SCK, true);
        bool value = gpio_get(MOSI);
        data[byte] |= value << (7-bit);
        gpio_put(SCK, false);
      }

      gpio_set_function(SCK,  GPIO_FUNC_SPI);
      gpio_set_function(MOSI, GPIO_FUNC_SPI);
    }

    gpio_put(CS, 1);
  }

  void SSD1680::command(uint8_t reg, size_t len, const uint8_t *data) {
    gpio_put(CS, 0);

    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi, &reg, 1);

    if(len > 0) {
      gpio_put(DC, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
    }

    gpio_put(CS, 1);
  }

  void SSD1680::data(size_t len, const uint8_t *data) {
    gpio_put(CS, 0);
    gpio_put(DC, 1); // data mode
    spi_write_blocking(spi, (const uint8_t*)data, len);
    gpio_put(CS, 1);
  }

  void SSD1680::command(uint8_t reg, std::initializer_list<uint8_t> values) {
    command(reg, values.size(), (uint8_t *)values.begin());
  }

  
  void SSD1680::partial_update(PicoGraphics *graphics, Rect region) {
    // region.y is given in columns ("banks"), which are groups of 8 horiontal pixels
    // region.x is given in pixels

    /*
    uint8_t *fb = (uint8_t *)graphics->frame_buffer;

    if(blocking) {
      busy_wait();
    }

    int cols = region.h / 8;
    int y1 = region.y / 8;

    int rows = region.w;
    int x1 = region.x;

    uint8_t partial_window[7] = {
      (uint8_t)(region.y),
      (uint8_t)(region.y + region.h - 1),
      (uint8_t)(region.x >> 8),
      (uint8_t)(region.x & 0xff),
      (uint8_t)((region.x + region.w - 1) >> 8),
      (uint8_t)((region.x + region.w - 1) & 0xff),
      0b00000001  // PT_SCAN
    };
    command(PON); // turn on

    command(PTIN); // enable partial mode
    command(PTL, sizeof(partial_window), partial_window);

    command(DTM2);
    for (auto dx = 0; dx < rows; dx++) {
      int sx = dx + x1;
      int sy = y1;
      data(cols, &fb[sy + (sx * (height / 8))]);
    }
    command(DSP); // data stop

    command(DRF); // start display refresh

    if(blocking) {
      off();
    }
      */
  }
  

  void SSD1680::update(PicoGraphics *graphics) {
    uint8_t *fb = (uint8_t *)graphics->frame_buffer;

    uint8_t *bufA = (uint8_t *)fb;
    uint8_t *bufB = bufA + (width * height / 8);

    if(blocking) {
      busy_wait();
    }


    command(SRXC, {X_START});
    command(SRYC, {Y_START_L, Y_START_H});
    command(WRAM_R, (width * height) / 8, bufA);

    command(SRXC, {X_START});
    command(SRYC, {Y_START_L, Y_START_H});
    command(WRAM_BW, (width * height) / 8, bufB);

    command(BTST);
    command(DUC2, {0xC7});

    command(ADUS);

    if(blocking) {
      off();
    }
  }

  void SSD1680::off() {
    busy_wait();
    //command(POF); // turn off
  }

}
