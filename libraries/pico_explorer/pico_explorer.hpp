#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class PicoExplorer : public PicoGraphics {
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 240;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;

    static const uint8_t ADC0 = 0;
    static const uint8_t ADC1 = 1;
    static const uint8_t ADC2 = 2;

    static const uint8_t MOTOR1 = 0;
    static const uint8_t MOTOR2 = 1;

    static const uint8_t FORWARD = 0;
    static const uint8_t REVERSE = 1;
    static const uint8_t STOP    = 2;

    static const uint8_t GP0 = 0;
    static const uint8_t GP1 = 1;
    static const uint8_t GP2 = 2;
    static const uint8_t GP3 = 3;
    static const uint8_t GP4 = 4;
    static const uint8_t GP5 = 5;
    static const uint8_t GP6 = 6;
    static const uint8_t GP7 = 7;

    uint16_t *__fb;
  private:
    ST7789 screen;
    int8_t audio_pin = -1;

  public:
    PicoExplorer(uint16_t *buf);

    void init();
    void update();
    bool is_pressed(uint8_t button);

    float get_adc(uint8_t channel);

    void set_motor(uint8_t channel, uint8_t action, float speed = 0.0f);

    void set_audio_pin(uint8_t pin);
    void set_tone(uint16_t frequency, float duty = 0.2f);
  };

}