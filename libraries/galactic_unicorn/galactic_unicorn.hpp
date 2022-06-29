#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  class GalacticUnicorn {
  public:
    static const int WIDTH  = 53;
    static const int HEIGHT = 11;
    static const uint8_t SWITCH_A = 0;
    static const uint8_t SWITCH_B = 1;
    static const uint8_t SWITCH_C = 3;
    static const uint8_t SWITCH_D = 6;
    static const uint8_t SWITCH_E = 2;
    static const uint8_t SWITCH_VOLUME_UP = 21;
    static const uint8_t SWITCH_VOLUME_DOWN = 26;
    static const uint8_t SWITCH_BRIGHTNESS_UP = 7;
    static const uint8_t SWITCH_BRIGHTNESS_DOWN = 8;

  private:
    PIO bitstream_pio = pio0;
    uint bitstream_sm = 0;
    uint sm_offset = 0;
  public:
    ~GalacticUnicorn();

    void init();

    void clear();
    void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(int x, int y, uint8_t v);

    bool is_pressed(uint8_t button);
  };

}