#pragma once

#include "hardware/pio.h"
#include "pico_graphics.hpp"

namespace pimoroni {

  class GalacticUnicorn {
  public:
    static const int WIDTH  = 53;
    static const int HEIGHT = 11;

    // pin assignments
    static const uint8_t COLUMN_CLOCK           = 13;
    static const uint8_t COLUMN_DATA            = 14;
    static const uint8_t COLUMN_LATCH           = 15;
    static const uint8_t COLUMN_BLANK           = 16;

    static const uint8_t ROW_BIT_0              = 17;
    static const uint8_t ROW_BIT_1              = 18;
    static const uint8_t ROW_BIT_2              = 19;
    static const uint8_t ROW_BIT_3              = 20;
  
    static const uint8_t LIGHT_SENSOR           = 28;

    static const uint8_t MUTE                   = 22;

    static const uint8_t I2S_DATA               =  9;
    static const uint8_t I2S_BCLK               = 10;
    static const uint8_t I2S_LRCLK              = 11;

    static const uint8_t I2C_SDA                =  4;
    static const uint8_t I2C_SCL                =  5;

    static const uint8_t SWITCH_A               =  0;
    static const uint8_t SWITCH_B               =  1;
    static const uint8_t SWITCH_C               =  3;
    static const uint8_t SWITCH_D               =  6;

    static const uint8_t SWITCH_SLEEP           = 27;

    static const uint8_t SWITCH_VOLUME_UP       =  7;
    static const uint8_t SWITCH_VOLUME_DOWN     =  8;
    static const uint8_t SWITCH_BRIGHTNESS_UP   = 21;
    static const uint8_t SWITCH_BRIGHTNESS_DOWN = 26;

  private:
    PIO bitstream_pio = pio0;
    uint bitstream_sm = 0;
    uint bitstream_sm_offset = 0;

    PIO audio_pio = pio0;
    uint audio_sm = 0;
    uint audio_sm_offset = 0;

    uint16_t brightness = 256;
    uint16_t volume = 127;

  public:
    ~GalacticUnicorn();

    void init();
    static inline void pio_program_init(PIO pio, uint sm, uint offset);

    void clear();

    void update(PicoGraphics_PenRGB565 &graphics);
    void update(PicoGraphics_PenRGB888 &graphics);

    void set_brightness(float value);
    float get_brightness();
    void adjust_brightness(float delta);

    void set_volume(float value);
    float get_volume();
    void adjust_volume(float delta);


    void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(int x, int y, uint8_t v);

    uint16_t light();

    bool is_pressed(uint8_t button);

    void play_sample(uint8_t *data, uint32_t length);

  };

}