#pragma once

#include "../../drivers/st7789/st7789.hpp"
#include "../../libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class PicoDisplay : public PicoGraphics {
    uint16_t __fb[240 * 135];
    ST7789 screen;

  public:
    PicoDisplay();

    void set_backlight(uint8_t brightness) {screen.set_backlight(brightness);}
    void update() {screen.update();}

    void set_led(uint8_t r, uint8_t g, uint8_t b);
    bool is_pressed(uint8_t button);

    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;
  };

}