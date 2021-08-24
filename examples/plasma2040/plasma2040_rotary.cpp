#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "breakout_encoder.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
Press "B" to enable cycling.
Press "A" to change the encoder mode.
Press "Boot" to reset the effects back to default.
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 30;

// The speed that the LEDs will start cycling at
const int16_t DEFAULT_SPEED = 20;

// The hue (in degrees) that the LEDs will start at
const int16_t DEFAULT_HUE = 0;

// The angle (in degrees) from the hue, that the LEDs will end at
const int16_t DEFAULT_ANGLE = 120;

// The brightness (between 0 and 31) to set the LEDs to
const int16_t DEFAULT_BRIGHTNESS = 16;



// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);


Button user_sw(plasma2040::USER_SW, Polarity::ACTIVE_LOW, 0);
Button button_a(plasma2040::BUTTON_A, Polarity::ACTIVE_LOW, 0);
Button button_b(plasma2040::BUTTON_B, Polarity::ACTIVE_LOW, 0);

RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);

I2C i2c(BOARD::PICO_EXPLORER);
BreakoutEncoder enc(&i2c);

enum ENCODER_MODE {
  COLOUR,
  ANGLE,
  BRIGHTNESS,
  TIME
};

float wrap(float v, float min, float max) {
  if(v <= min)
    v += (max - min);

  if(v > max)
    v -= (max - min);

  return v;
}


void colour_cycle(float hue, float t, float angle) {
  t /= 200.0f;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    float percent_along = (float)i / led_strip.num_leds;
    float offset = sinf((percent_along + 0.5f + t) * M_PI) * angle;
    float h = wrap((hue + offset) / 360.0f, 0.0f, 1.0f);
    led_strip.set_hsv(i, h, 1.0f, 1.0f);
  }
}

void speed_gauge(uint v, uint vmax = 100) {
  uint light_pixels = led_strip.num_leds * v / vmax;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    if(i < light_pixels) {
      led_strip.set_rgb(i, 0, 255, 0);
    }
    else {
      led_strip.set_rgb(i, 255, 0, 0);
    }
  }
}

void brightness_gauge(uint v, uint vmax = 100) {
  uint light_pixels = led_strip.num_leds * v / vmax;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    if(i < light_pixels) {
      led_strip.set_rgb(i, 64, 64, 64);
    }
    else {
      led_strip.set_rgb(i, 0, 0, 0);
    }
  }
}

int main() {
  stdio_init_all();

  led_strip.start(UPDATES);

  bool encoder_detected = enc.init();
  enc.clear_interrupt_flag();

  //Initialise the default values
  int16_t speed = DEFAULT_SPEED;
  int16_t hue = DEFAULT_HUE;
  int16_t angle = DEFAULT_ANGLE;
  int16_t brightness = DEFAULT_BRIGHTNESS;

  bool cycle = true;
  ENCODER_MODE mode = ENCODER_MODE::COLOUR;
  uint32_t start_time = millis();
  while(true) {
    uint32_t t = millis() - start_time;
    if(encoder_detected) {
      if(enc.get_interrupt_flag()) {
        int16_t count = enc.read();
        enc.clear_interrupt_flag();
        enc.clear();

        cycle = false;
        switch(mode) {
          case ENCODER_MODE::COLOUR:
            hue += count;
            hue = std::min((int16_t)359, std::max((int16_t)0, hue));
            colour_cycle((float)hue, 0, (float)angle);
            break;

          case ENCODER_MODE::ANGLE:
            angle += count;
            angle = std::min((int16_t)359, std::max((int16_t)0, angle));
            colour_cycle((float)hue, 0, (float)angle);
            break;

          case ENCODER_MODE::BRIGHTNESS:
            brightness += count;
            brightness = std::min((int16_t)31, std::max((int16_t)0, brightness));
            led_strip.set_brightness((uint8_t)brightness);
            brightness_gauge(brightness, 31);
            break;

          case ENCODER_MODE::TIME:
            speed += count;
            speed = std::min((int16_t)100, std::max((int16_t)0, speed));
            speed_gauge(speed, 100);
            break;
        }
      }
    }
    bool sw_pressed = user_sw.read();
    bool a_pressed = button_a.read();
    bool b_pressed = button_b.read();

    if(sw_pressed) {
      speed = DEFAULT_SPEED;
      hue = DEFAULT_HUE;
      angle = DEFAULT_ANGLE;
      brightness = DEFAULT_BRIGHTNESS;
    }

    if(b_pressed) {
      if(!cycle)
        start_time = millis();
      cycle = true;
    }

    switch(mode) {
      case ENCODER_MODE::COLOUR:
        led.set_rgb(255, 0, 0);
        if(a_pressed) mode = ENCODER_MODE::ANGLE;
        break;

      case ENCODER_MODE::ANGLE:
        led.set_rgb(255, 255, 0);
        if(a_pressed) mode = ENCODER_MODE::BRIGHTNESS;
        break;

      case ENCODER_MODE::BRIGHTNESS:
        led.set_rgb(0, 255, 0);
        if(a_pressed) mode = ENCODER_MODE::TIME;
        break;

      case ENCODER_MODE::TIME:
        led.set_rgb(0, 0, 255);
        if(a_pressed) mode = ENCODER_MODE::COLOUR;
        break;
    }

    if(cycle)
      colour_cycle(hue, (float)(t * speed) / 100.0f, (float)angle);

    auto first_led = led_strip.get(led_strip.num_leds / 2);
    enc.set_led(first_led.r, first_led.g, first_led.b);

    // Sleep time controls the rate at which the LED buffer is updated
    // but *not* the actual framerate at which the buffer is sent to the LEDs
    sleep_ms(1000 / UPDATES);
  }
}
