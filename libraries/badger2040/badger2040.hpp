#pragma once

#include <string>

#include "drivers/uc8151/uc8151.hpp"

#include "fonts.hpp"

namespace pimoroni {

  class Badger2040 {
  private:
    UC8151 uc8151;
    const hershey_font_t *_font = &futural;
    uint8_t _pen = 0;
    uint8_t _thickness = 1;
    uint32_t _button_states = 0;
    uint32_t _wake_button_states = 0;
  private:

  public:
    Badger2040();
    void init();
    void update();
    void halt();
    void sleep();

    // state
    void led(uint8_t brightness);
    void font(std::string name);
    void pen(uint8_t pen);
    void thickness(uint8_t thickness);

    // inputs (buttons: A, B, C, D, E, USER)
    bool pressed(uint8_t button);
    bool pressed_to_wake(uint8_t button);
    void wait_for_press();
    void update_button_states();
    uint32_t button_states();

    // drawing primitives
    void clear();
    void pixel(int32_t x, int32_t y);
    void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void rectangle(int32_t x, int32_t y, int32_t w, int32_t h);

    // text (fonts: sans, sans_bold, gothic, cursive_bold, cursive, serif_italic, serif, serif_bold)
    void text(std::string message, int32_t x, int32_t y, float s = 1.0f);
    const hershey_font_glyph_t* glyph_data(unsigned char c);
    int32_t glyph(unsigned char c, int32_t x, int32_t y, float s);

  public:
    enum pin {
      A           = 12,
      B           = 13,
      C           = 14,
      D           = 15,
      E           = 11,
      UP          = 15, // alias for D
      DOWN        = 11, // alias for E
      USER        = 23,
      CS          = 17,
      CLK         = 18,
      MOSI        = 19,
      DC          = 20,
      RESET       = 21,
      BUSY        = 26,
      VBUS_DETECT = 24,
      LED         = 25,
      BATTERY     = 29,
      ENABLE_3V3  = 10
    };

  };

}