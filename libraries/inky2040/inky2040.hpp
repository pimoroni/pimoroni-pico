#pragma once

#include <string>

#include "drivers/uc8159/uc8159.hpp"

#include "libraries/hershey_fonts/hershey_fonts.hpp"
#include "libraries/bitmap_fonts/bitmap_fonts.hpp"
#include "libraries/bitmap_fonts/font6_data.hpp"
#include "libraries/bitmap_fonts/font8_data.hpp"

namespace pimoroni {

  class Inky2040 {
  protected:
    UC8159 uc8159;
    const hershey::font_t *_font = &hershey::futural;
    const bitmap::font_t *_bitmap_font = nullptr;
    uint8_t _pen = 0;
    uint8_t _thickness = 1;
    uint32_t _button_states = 0;
    uint32_t _wake_button_states = 0;
  private:

  public:
    static const int WIDTH = 600;
    static const int HEIGHT = 448;

    Inky2040()
      : uc8159(WIDTH, HEIGHT, spi0, CS, DC, CLK, MOSI, BUSY, RESET) {
    };
    // Constructor for Python-managed buffer
    Inky2040(uint8_t *framebuffer)
      : uc8159(WIDTH, HEIGHT, framebuffer, spi0, CS, DC, CLK, MOSI, BUSY, RESET) {
    };
    void init();
    void update(bool blocking=false);
    void halt();
    void sleep();
    bool is_busy();
    void power_off();

    // state
    void led(uint8_t brightness);
    void font(std::string name);
    void pen(uint8_t pen);
    void thickness(uint8_t thickness);

    // inputs (buttons: A, B, C, D, E, USER)
    //bool pressed(uint8_t button);
    //bool pressed_to_wake(uint8_t button);
    //void wait_for_press();
    //void update_button_states();
    //uint32_t button_states();

    // drawing primitives
    void clear();
    void pixel(int32_t x, int32_t y);
    void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void rectangle(int32_t x, int32_t y, int32_t w, int32_t h);

    void icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy);
    void image(const uint8_t* data);
    void image(const uint8_t *data, int w, int h, int x, int y);
    void image(const uint8_t *data, int stride, int sx, int sy, int dw, int dh, int dx, int dy);

    // text (fonts: sans, sans_bold, gothic, cursive_bold, cursive, serif_italic, serif, serif_bold)
    const hershey::font_glyph_t* glyph_data(unsigned char c);

    void text(std::string message, int32_t x, int32_t y, float s = 1.0f, float a = 0.0f);
    int32_t glyph(unsigned char c, int32_t x, int32_t y, float s = 1.0f, float a = 0.0f);

    int32_t measure_text(std::string message, float s = 1.0f);
    int32_t measure_glyph(unsigned char c, float s = 1.0f);

  public:
    enum pin {
      A           = 12,
      B           = 13,
      C           = 14,
      D           = 15,
      E           = 11,
      USER        = 23,
      CS          = 17,
      CLK         = 18,
      MOSI        = 19,
      DC          = 27,
      RESET       = 25,
      BUSY        = 26,
      VBUS_DETECT = 24,
      LED         = 25,
      BATTERY     = 29,
      ENABLE_3V3  = 10
    };

  };

}
