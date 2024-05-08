#include "st7701.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {
  uint8_t madctl;

  enum dcx {
    CMD = 0x00,
    DATA = 0x01
  };

  enum reg {
    SWRESET = 0x01, // Software Reset
    SLPOUT = 0x11,  // Sleep Out
    PTLON = 0x12,   // Partial Display Mode On
    NORON = 0x13,   // Normal Display Mode On
    INVOFF = 0x20,  // Display Inversion Off
    INVON = 0x21,   // Display Inversion On
    ALLPOFF = 0x22, // All Pixels Off
    ALLPON = 0x23,  // All Pixels On
    GAMSET = 0x26,  // Gamma Set
    DISPOFF = 0x28, // Display Off
    DISPON = 0x29,  // Display On
    TEOFF = 0x34,   // Tearing Effect Line Off (kinda vsync)
    TEON = 0x35,    // Tearing Effect Line On (kinda vsync)
    MADCTL = 0x36,  // Display data access control
    IDMOFF = 0x38,  // Idle Mode Off
    IDMON = 0x39,   // Idle Mode On
    COLMOD = 0x3A,  // Interface Pixel Format
    GSL = 0x45,     // Get Scan Line
    // Command2_BK0
    PVGAMCTRL = 0xB0,  // Positive Voltage Gamma Control
    NVGAMCTRL = 0xB1,  // Negative Voltage Gamma Control
    DGMEN = 0xB8,   // Digital Gamma Enable
    DGMLUTR = 0xB9, // Digital Gamma LUT for Red
    DGMLUTB = 0xBA, // Digital Gamma Lut for Blue
    LNESET = 0xC0,  // Display Line Setting
    PORCTRL = 0xC1, // Porch Control
    INVSET = 0xC2,  // Inversion Selection & Frame Rate Control
    RGBCTRL = 0xC3, // RGB Control
    PARCTRL = 0xC5, // Partial Mode Control
    SDIR = 0xC7,    // X-direction Control
    PDOSET = 0xC8,  // Pseudo-Dot Inversion Diving Settign
    COLCTRL = 0xCD, // Colour Control
    SRECTRL = 0xE0, // Sunlight Readable Enhancement
    NRCTRL = 0xE1,  // Noise Reduce Control
    SECTRL = 0xE2,  // Sharpness Control
    CCCTRL = 0xE3,  // Color Calibration Control
    SKCTRL = 0xE4,  // Skin Tone Preservation Control
    // Command2_BK1
    VHRS = 0xB0,    // Vop amplitude
    VCOMS = 0xB1,   // VCOM amplitude
    VGHSS = 0xB2,   // VGH voltage
    TESTCMD = 0xB3, // TEST command
    VGLS = 0xB5,    // VGL voltage
    VRHDV = 0xB6,   // VRH_DV voltage
    PWCTRL1 = 0xB7, // Power Control 1
    PWCTRL2 = 0xB8, // Power Control 2
    PCLKS1 = 0xBA,  // Power pumping clock selection 1
    PCLKS2 = 0xBC,  // Power pumping clock selection 2
    PDR1 = 0xC1,    // Source pre_drive timing set 1
    PDR2 = 0xC2,    // Source pre_drive timing set 2
    // Command2_BK3
    NVMEN = 0xC8,    // NVM enable
    NVMSET = 0xCA,   // NVM manual control
    PROMACT = 0xCC,  // NVM program active
    // Other
    CND2BKxSEL = 0xFF,
  };

  void ST7701::common_init() {
    // if a backlight pin is provided then set it up for
    // pwm control
    if(lcd_bl != PIN_UNUSED) {
      pwm_config cfg = pwm_get_default_config();
      pwm_set_wrap(pwm_gpio_to_slice_num(lcd_bl), 65535);
      pwm_init(pwm_gpio_to_slice_num(lcd_bl), &cfg, true);
      gpio_set_function(lcd_bl, GPIO_FUNC_PWM);
      set_backlight(0); // Turn backlight off initially to avoid nasty surprises
    }

    command(reg::SWRESET);

    sleep_ms(150);

    // Commmand 2 BK0 - kinda a page select
    command(reg::CND2BKxSEL, 5, "\x77\x01\x00\x00\x10");

    if(width == 480 && height == 480) {
      // TODO: Figure out what's actually display specific
      command(reg::MADCTL, 1, "\x00");  // Normal scan direction and RGB pixels
      command(reg::LNESET, 2, "\x3b\x00");   // (59 + 1) * 8 = 480 lines
      command(reg::PORCTRL, 2, "\x0d\x02");  // 13 VBP, 2 VFP
      command(reg::INVSET, 2, "\x32\x05");
      command(reg::COLCTRL, 1, "\x08");      // LED polarity reversed
      command(reg::PVGAMCTRL, 16, "\x00\x11\x18\x0e\x11\x06\x07\x08\x07\x22\x04\x12\x0f\xaa\x31\x18");
      command(reg::NVGAMCTRL, 16, "\x00\x11\x19\x0e\x12\x07\x08\x08\x08\x22\x04\x11\x11\xa9\x32\x18");
    }

    // Command 2 BK1 - Voltages and power and stuff
    command(reg::CND2BKxSEL, 5, "\x77\x01\x00\x00\x11");
    command(reg::VHRS, 1, "\x60");    // 4.7375v
    command(reg::VCOMS, 1, "\x32");   // 0.725v
    command(reg::VGHSS, 1, "\x07");   // 15v
    command(reg::TESTCMD, 1, "\x80"); // y tho?
    command(reg::VGLS, 1, "\x49");    // -10.17v
    command(reg::PWCTRL1, 1, "\x85"); // Middle/Min/Min bias
    command(reg::PWCTRL2, 1, "\x21"); // 6.6 / -4.6
    command(reg::PDR1, 1, "\x78");    // 1.6uS
    command(reg::PDR2, 1, "\x78");    // 6.4uS

    // Begin Forbidden Knowledge
    // This sequence is probably specific to TL040WVS03CT15-H1263A.
    // It is not documented in the ST7701s datasheet.
    // TODO: 👇 W H A T ! ? 👇
    command(0xE0, 3, "\x00\x1b\x02");
    command(0xE1, 11, "\x08\xa0\x00\x00\x07\xa0\x00\x00\x00\x44\x44");
    command(0xE2, 12, "\x11\x11\x44\x44\xed\xa0\x00\x00\xec\xa0\x00\x00");
    command(0xE3, 4, "\x00\x00\x11\x11");
    command(0xE4, 2, "\x44\x44");
    command(0xE5, 16, "\x0a\xe9\xd8\xa0\x0c\xeb\xd8\xa0\x0e\xed\xd8\xa0\x10\xef\xd8\xa0");
    command(0xE7, 2, "\x44\x44");
    command(0xE8, 16, "\x09\xe8\xd8\xa0\x0b\xea\xd8\xa0\x0d\xec\xd8\xa0\x0f\xee\xd8\xa0");
    command(0xEC, 2, "\x3c\x00");
    command(0xED, 16, "\xab\x89\x76\x54\x02\xff\xff\xff\xff\xff\xff\x20\x45\x67\x98\xba");
    command(0x36, 1, "\x00");
    // End Forbidden Knowledge

    // Command 2 BK3
    command(reg::CND2BKxSEL, 5, "\x77\x01\x00\x00\x13");
    //command(reg::COLMOD, 1, "\x77");  // 24 bits per pixel...
    command(reg::COLMOD, 1, "\x66");    // 18 bits per pixel...
    //command(reg::COLMOD, 1, "\x55");  // 16 bits per pixel...

    command(reg::SLPOUT);
    sleep_ms(120);
    command(reg::DISPON);
    sleep_ms(50);

    // TODO: Support rotation
    // configure_display(rotation);

    if(lcd_bl != PIN_UNUSED) {
      //update(); // Send the new buffer to the display to clear any previous content
      sleep_ms(50); // Wait for the update to apply
      set_backlight(255); // Turn backlight on now surprises have passed
    }
  }

  void ST7701::cleanup() {
    if(dma_channel_is_claimed(st_dma)) {
      dma_channel_abort(st_dma);
      dma_channel_unclaim(st_dma);
    }
    if(spi) return; // SPI mode needs no further tear down

    if(pio_sm_is_claimed(parallel_pio, parallel_sm)) {
      pio_sm_set_enabled(parallel_pio, parallel_sm, false);
      pio_sm_drain_tx_fifo(parallel_pio, parallel_sm);
      pio_sm_unclaim(parallel_pio, parallel_sm);
    }
  }

  void ST7701::configure_display(Rotation rotate) {

    if(rotate == ROTATE_90 || rotate == ROTATE_270) {
      std::swap(width, height);
    }

    // 480x480 Square Display
    if(width == 480 && height == 480) {
      madctl = 0;
    }

    command(reg::MADCTL, 1, (char *)&madctl);
  }

  void ST7701::write_blocking_dma(const uint8_t *src, size_t len) {
    while (dma_channel_is_busy(st_dma))
      ;
    dma_channel_set_trans_count(st_dma, len, false);
    dma_channel_set_read_addr(st_dma, src, true);
  }

  void ST7701::write_blocking_parallel(const uint8_t *src, size_t len) {
    write_blocking_dma(src, len);
    dma_channel_wait_for_finish_blocking(st_dma);

    // This may cause a race between PIO and the
    // subsequent chipselect deassert for the last pixel
    while(!pio_sm_is_tx_fifo_empty(parallel_pio, parallel_sm))
      ;
  }

  void ST7701::command(uint8_t command, size_t len, const char *data) {
    static uint16_t _data[20] = {0};
    gpio_put(spi_cs, 0);
    
    // Add leading byte for 9th D/CX bit
    uint8_t _command[2] = {dcx::CMD, command};
    spi_write_blocking(spi, (const uint8_t*)_command, 2);

    if(data) {
      // Add leading bytes for 9th D/CX bits
      // TODO: OOOF - I *think* this is how 9bit SPI is supposed to work!?
      // We'd probably be better off with some dedicated PIO tomfoolery
      for(auto i = 0u; i < len; i++) {
        _data[i] = (dcx::DATA << 8) | data[i];
      }
      spi_write_blocking(spi, (const uint8_t*)_data, len);
    }

    gpio_put(spi_cs, 1);
  }
  
  void ST7701::update(PicoGraphics *graphics) {
    //uint8_t cmd = reg::RAMWR;

    // TODO: Where's my buffer at? Where's my buffer at?
    // I left it out back in the PSRAM chip, y'all
    // And now I cannot find it even though it's 60k tall

    if(graphics->pen_type == PicoGraphics::PEN_RGB565) { // Display buffer is screen native
      // command(cmd, width * height * sizeof(uint16_t), (const char*)graphics->frame_buffer);
    } else {
      /*graphics->frame_convert(PicoGraphics::PEN_RGB565, [this](void *data, size_t length) {
        if (length > 0) {
          write_blocking_dma((const uint8_t*)data, length);
        }
        else {
          dma_channel_wait_for_finish_blocking(st_dma);
        }
      });*/
    }
  }

  void ST7701::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(lcd_bl, value);
  }
}
