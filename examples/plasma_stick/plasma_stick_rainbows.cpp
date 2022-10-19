#include "pico/stdlib.h"
#include "plasma_stick.hpp"
#include "common/pimoroni_common.hpp"

/*
Make some rainbows!
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint NUM_LEDS = 50;

// The SPEED that the LEDs cycle at (1 - 255)
const uint SPEED = 20;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// WS2812 / NeoPixel™ LEDs
WS2812 led_strip(NUM_LEDS, pio0, 0, plasma_stick::DAT);


int main() {
  stdio_init_all();

  // Start updating the LED strip
  led_strip.start(UPDATES);

  float offset = 0.0f;

  // Make rainbows
  while(true) {

    offset += float(SPEED) / 2000.0f;

    for(auto i = 0u; i < NUM_LEDS; ++i) {
      float hue = float(i) / NUM_LEDS;
      led_strip.set_hsv(i, hue + offset, 1.0f, 1.0f);
    }

    sleep_ms(1000 / UPDATES);
  }
}
