#pragma once

namespace pimoroni {

  class PicoScroll {
    uint8_t __fb[144];

  public:
    void init();
    void update();
    void set_pixel(uint8_t x, uint8_t y, uint8_t v);
    void clear();
    bool is_pressed(uint8_t button);

    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;
  };

}