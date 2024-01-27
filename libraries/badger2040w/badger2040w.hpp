#pragma once

#include <string>

#include "drivers/uc8151/uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class Badger2040W {
  protected:
    uint32_t _button_states = 0;
    uint32_t _wake_button_states = 0;
  private:

  public:
    UC8151* uc8151 = nullptr;
    PicoGraphics_Pen1BitY* graphics = nullptr;
    Badger2040W(){};
    void init();
    void update();
    void partial_update(Rect region);
    void halt();

    // state
    void led(uint8_t brightness);

    // inputs (buttons: A, B, C, D, E, USER)
    bool pressed(uint8_t button);
    bool pressed_to_wake(uint8_t button);
    void wait_for_press();
    void update_button_states();
    uint32_t button_states();

  public:
    enum pin {
      A           = 12,
      B           = 13,
      C           = 14,
      D           = 15,
      E           = 11,
      UP          = 15, // alias for D
      DOWN        = 11, // alias for E
      CS          = 17,
      CLK         = 18,
      MOSI        = 19,
      DC          = 20,
      RESET       = 21,
      BUSY        = 26,
      VBUS_DETECT = 24,
      LED         = 22,
      BATTERY     = 29,
      ENABLE_3V3  = 10
    };

  };

}
