#pragma once

namespace pimoroni {

  class PicoRGBKeypad {
  private:
    static const uint8_t KEYPAD_ADDRESS = 0x20;
    static constexpr float DEFAULT_BRIGHTNESS = 0.5f;
  public:
    static const int WIDTH = 4;
    static const int HEIGHT = 4;
    static const int NUM_PADS = WIDTH * HEIGHT;

  private:
    uint8_t buffer[(NUM_PADS * 4) + 8];
    uint8_t *led_data;
  
  public:
    ~PicoRGBKeypad();
    void init();
    void update();
    void set_brightness(float brightness);
    void illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b);

    void clear();
    uint16_t get_button_states();
  };

}