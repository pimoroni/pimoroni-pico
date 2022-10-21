#include <stdio.h>
#include "pico/stdlib.h"
#include "plasma_stick.hpp"
#include "common/pimoroni_common.hpp"
#include "breakout_encoder.hpp"

/*
Change the colour of your LEDs easily by hooking up an RGB Encoder Breakout!
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint NUM_LEDS = 50;

// Make this number bigger for more precise colour adjustments
const uint STEPS_PER_REV = 24;

// Set up the WS2812 / NeoPixel™ LEDs, with RGB color order to work with the LED wire that comes with Skully
WS2812 led_strip(NUM_LEDS, pio0, 0, plasma_stick::DAT, WS2812::DEFAULT_SERIAL_FREQ, false, WS2812::COLOR_ORDER::RGB);

I2C i2c(BOARD::PLASMA_STICK);
BreakoutEncoder enc(&i2c);

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

void count_changed(int16_t count) {
  printf("Count: %d\n", count);
  float h = (count % STEPS_PER_REV) / (float)STEPS_PER_REV;
  uint8_t r = 0, g = 0, b = 0;
  from_hsv(h, 1.0f, 1.0f, r, g, b);

  // set the encoder LED colour
  enc.set_led(r, g, b);

  for(auto i = 0u; i < NUM_LEDS; i++) {
    led_strip.set_rgb(i, r, g, b);
  }
}

int main() {
  stdio_init_all();

  led_strip.start();

  if(enc.init()) {
    printf("Encoder found...\n");

    enc.set_brightness(1.0f);
    //enc.set_direction(BreakoutEncoder::DIRECTION_CCW);     // Uncomment this to flip the direction

    count_changed(0);
    enc.clear_interrupt_flag();

    while(true) {
      if(enc.get_interrupt_flag()) {
        int16_t count = enc.read();
        enc.clear_interrupt_flag();
        
        while(count < 0) {
          count += STEPS_PER_REV;
        }

        count_changed(count);
      }
    }
  }
  else {
    printf("Encoder not found :'(\n");
  }

  return 0;
}
