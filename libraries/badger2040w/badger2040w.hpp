#pragma once

#include <string>
#include <memory>

#include "drivers/uc8151/uc8151.hpp"
#include "drivers/pcf85063a/pcf85063a.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class Badger2040W {
  protected:
    uint32_t _button_states = 0;
    uint32_t _wake_button_states = 0;
  private:
    void imageRow(const uint8_t *data, Rect rect);

  public:
    std::unique_ptr<UC8151> uc8151;
    std::unique_ptr<PicoGraphics_Pen1BitY> graphics;
    std::unique_ptr<PCF85063A> pcf85063a;
    uint DISPLAY_WIDTH = 296;
    uint DISPLAY_HEIGHT = 128;
    
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

    void icon(const uint8_t *data, int index, int sheet_width, Rect rect);
    void image(const uint8_t *data, Rect rect);
    void image(const uint8_t *data);

  public:
    enum pin {
      RTC         = 8,
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
