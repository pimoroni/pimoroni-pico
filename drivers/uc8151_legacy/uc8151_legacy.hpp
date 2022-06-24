#pragma once

#include <initializer_list>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class UC8151_Legacy {
    enum PSR_FLAGS {
      RES_96x230   = 0b00000000,
      RES_96x252   = 0b01000000,
      RES_128x296  = 0b10000000,
      RES_160x296  = 0b11000000,

      LUT_OTP      = 0b00000000,
      LUT_REG      = 0b00100000,

      FORMAT_BWR   = 0b00000000,
      FORMAT_BW    = 0b00010000,

      SCAN_DOWN    = 0b00000000,
      SCAN_UP      = 0b00001000,

      SHIFT_LEFT   = 0b00000000,
      SHIFT_RIGHT  = 0b00000100,

      BOOSTER_OFF  = 0b00000000,
      BOOSTER_ON   = 0b00000010,

      RESET_SOFT   = 0b00000000,
      RESET_NONE   = 0b00000001
    };

    enum PWR_FLAGS_1 {
      VDS_EXTERNAL = 0b00000000,
      VDS_INTERNAL = 0b00000010,

      VDG_EXTERNAL = 0b00000000,
      VDG_INTERNAL = 0b00000001
    };

    enum PWR_FLAGS_2 {
      VCOM_VD      = 0b00000000,
      VCOM_VG      = 0b00000100,

      VGHL_16V     = 0b00000000,
      VGHL_15V     = 0b00000001,
      VGHL_14V     = 0b00000010,
      VGHL_13V     = 0b00000011
    };

    enum BOOSTER_FLAGS {
      START_10MS = 0b00000000,
      START_20MS = 0b01000000,
      START_30MS = 0b10000000,
      START_40MS = 0b11000000,

      STRENGTH_1 = 0b00000000,
      STRENGTH_2 = 0b00001000,
      STRENGTH_3 = 0b00010000,
      STRENGTH_4 = 0b00011000,
      STRENGTH_5 = 0b00100000,
      STRENGTH_6 = 0b00101000,
      STRENGTH_7 = 0b00110000,
      STRENGTH_8 = 0b00111000,

      OFF_0_27US = 0b00000000,
      OFF_0_34US = 0b00000001,
      OFF_0_40US = 0b00000010,
      OFF_0_54US = 0b00000011,
      OFF_0_80US = 0b00000100,
      OFF_1_54US = 0b00000101,
      OFF_3_34US = 0b00000110,
      OFF_6_58US = 0b00000111
    };

    enum PFS_FLAGS {
      FRAMES_1   = 0b00000000,
      FRAMES_2   = 0b00010000,
      FRAMES_3   = 0b00100000,
      FRAMES_4   = 0b00110000
    };

    enum TSE_FLAGS {
      TEMP_INTERNAL = 0b00000000,
      TEMP_EXTERNAL = 0b10000000,

      OFFSET_0      = 0b00000000,
      OFFSET_1      = 0b00000001,
      OFFSET_2      = 0b00000010,
      OFFSET_3      = 0b00000011,
      OFFSET_4      = 0b00000100,
      OFFSET_5      = 0b00000101,
      OFFSET_6      = 0b00000110,
      OFFSET_7      = 0b00000111,

      OFFSET_MIN_8  = 0b00001000,
      OFFSET_MIN_7  = 0b00001001,
      OFFSET_MIN_6  = 0b00001010,
      OFFSET_MIN_5  = 0b00001011,
      OFFSET_MIN_4  = 0b00001100,
      OFFSET_MIN_3  = 0b00001101,
      OFFSET_MIN_2  = 0b00001110,
      OFFSET_MIN_1  = 0b00001111
    };

    enum PLL_FLAGS {
      // other frequency options exist but there doesn't seem to be much
      // point in including them - this is a fair range of options...
      HZ_29      = 0b00111111,
      HZ_33      = 0b00111110,
      HZ_40      = 0b00111101,
      HZ_50      = 0b00111100,
      HZ_67      = 0b00111011,
      HZ_100     = 0b00111010,
      HZ_200     = 0b00111001
    };

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;

    // highest possible resolution is 160x296 which at 1 bit per pixel
    // requires 5920 bytes of frame buffer
    //uint8_t frame_buffer[5920] = {0};
    uint8_t *frame_buffer;

    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    // interface pins with our standard defaults where appropriate
    uint CS     = SPI_BG_FRONT_CS;
    uint DC     = SPI_DEFAULT_MISO;
    uint SCK    = SPI_DEFAULT_SCK;
    uint MOSI   = SPI_DEFAULT_MOSI;
    uint BUSY   = PIN_UNUSED;
    uint RESET  = PIN_UNUSED;

    bool inverted = false;

    uint8_t _update_speed = 0;

  public:
    UC8151_Legacy(uint16_t width, uint16_t height) :
      width(width), height(height), frame_buffer(new uint8_t[width * height / 8]) {
    }

    UC8151_Legacy(uint16_t width, uint16_t height, uint8_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {
    }

    UC8151_Legacy(uint16_t width, uint16_t height,
           spi_inst_t *spi,
           uint CS, uint DC, uint SCK, uint MOSI,
           uint BUSY = PIN_UNUSED, uint RESET = PIN_UNUSED) :
      width(width), height(height),
      frame_buffer(new uint8_t[width * height / 8]),
      spi(spi),
      CS(CS), DC(DC), SCK(SCK), MOSI(MOSI), BUSY(BUSY), RESET(RESET) {}

    UC8151_Legacy(uint16_t width, uint16_t height,
           uint8_t *frame_buffer,
           spi_inst_t *spi,
           uint CS, uint DC, uint SCK, uint MOSI,
           uint BUSY = PIN_UNUSED, uint RESET = PIN_UNUSED) :
      width(width), height(height),
      frame_buffer(frame_buffer),
      spi(spi),
      CS(CS), DC(DC), SCK(SCK), MOSI(MOSI), BUSY(BUSY), RESET(RESET) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void busy_wait();
    bool is_busy();
    void reset();
    void setup(uint8_t speed=0);
    void power_off();

    void default_luts();
    void medium_luts();
    void fast_luts();
    void turbo_luts();

    void read(uint8_t reg, size_t len, uint8_t *data);
    void command(uint8_t reg, size_t len, const uint8_t *data);
    void command(uint8_t reg, std::initializer_list<uint8_t> values);
    void command(uint8_t reg) {command(reg, 0, nullptr);};
    void data(size_t len, const uint8_t *data);

    void invert(bool invert);
    void update_speed(uint8_t speed);
    uint8_t update_speed();
    uint32_t update_time();
    void update(bool blocking = true);
    void partial_update(int x, int y, int w, int h, bool blocking = true);
    void off();

    void pixel(int x, int y, int v);
    uint8_t* get_frame_buffer();
  };

}
