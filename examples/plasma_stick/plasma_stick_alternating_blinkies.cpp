#include "pico/stdlib.h"
#include "plasma_stick.hpp"
#include "common/pimoroni_common.hpp"

/*
This super simple example sets up two alternating colours, great for festive lights!
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint NUM_LEDS = 50;

// Pick two hues from the colour wheel (from 0-360°, try https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/ )
constexpr float HUE_1 = 40.0f;
constexpr float HUE_2 = 285.0f;

// Set up brightness (between 0 and 1)
constexpr float BRIGHTNESS = 0.5f;

// Set up speed (wait time between colour changes, in seconds)
constexpr float SPEED = 1.0f;

// WS2812 / NeoPixel™ LEDs
WS2812 led_strip(NUM_LEDS, pio0, 0, plasma_stick::DAT);


int main() {
  stdio_init_all();

  // Start updating the LED strip
  led_strip.start();

  while(true) {

    for(auto i = 0u; i < NUM_LEDS; i++) {
        // the if statements below use a modulo operation to identify the even and odd numbered LEDs
        if((i % 2) == 0)
            led_strip.set_hsv(i, HUE_1 / 360, 1.0, BRIGHTNESS);
        else
            led_strip.set_hsv(i, HUE_2 / 360, 1.0, BRIGHTNESS);
    }
    sleep_ms(SPEED * 1000.0f);

    for(auto i = 0u; i < NUM_LEDS; i++) {
        if((i % 2) == 0)
            led_strip.set_hsv(i, HUE_2 / 360, 1.0, BRIGHTNESS);
        else
            led_strip.set_hsv(i, HUE_1 / 360, 1.0, BRIGHTNESS);
    }
    sleep_ms(SPEED * 1000.0f);
  }
}
