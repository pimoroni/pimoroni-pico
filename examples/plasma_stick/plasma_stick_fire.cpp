#include "pico/stdlib.h"
#include "plasma_stick.hpp"
#include "common/pimoroni_common.hpp"

/*
A basic fire effect.
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint NUM_LEDS = 50;

// Set up the WS2812 / NeoPixel™ LEDs, with RGB color order to work with the LED wire that comes with Skully
WS2812 led_strip(NUM_LEDS, pio0, 0, plasma_stick::DAT, WS2812::DEFAULT_SERIAL_FREQ, false, WS2812::COLOR_ORDER::RGB);

// Maps a value from one range to another
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main() {
  stdio_init_all();

  // Start updating the LED strip
  led_strip.start();

  while(true) {
    // fire effect! Random red/orange hue, full saturation, random brightness
    for(auto i = 0u; i < NUM_LEDS; ++i) {
      float hue = map((float)rand(), 0.0f, (float)RAND_MAX, 0.0, 50.0f / 360.0f);
      float brightness = map((float)rand(), 0.0f, (float)RAND_MAX, 0.0, 1.0f);
      led_strip.set_hsv(i, hue, 1.0, brightness);
    }
    sleep_ms(100);
  }
}
