#include <stdio.h>
#include "pico/stdlib.h"
#include "plasma_stick.hpp"
#include "common/pimoroni_common.hpp"
#include "bme280.hpp"

/*
Reads the temperature from a BME280 breakout...
...and changes the LED strip an appropriate colour.
https://shop.pimoroni.com/products/bme280-breakout
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint NUM_LEDS = 50;

// Set up brightness (between 0 and 1)
constexpr float BRIGHTNESS = 1.0f;

// The range of readings that we want to map to colours
const uint MIN = 10;
const uint MAX = 30;

// Pick what bits of the colour wheel to use (from 0-360°)
// https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
const uint HUE_START = 230; // blue
const uint HUE_END = 359;  // red

// Set up the WS2812 / NeoPixel™ LEDs, with RGB color order to work with the LED wire that comes with Skully
WS2812 led_strip(NUM_LEDS, pio0, 0, plasma_stick::DAT, WS2812::DEFAULT_SERIAL_FREQ, false, WS2812::COLOR_ORDER::RGB);

I2C i2c(BOARD::PLASMA_STICK);
BME280 bme(&i2c);


int main() {
  stdio_init_all();

  led_strip.start();

  if(bme.init()) {
    printf("BME280 found...\n");

    while(true) {
      BME280::bme280_reading result = bme.read_forced();
      printf("%s %0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", result.status == BME280_OK ? "OK" : "ER", result.temperature, result.pressure, result.humidity);
      
      // calculates a colour
      float hue = HUE_START + ((float)(result.temperature - MIN) * (float)(HUE_END - HUE_START) / (float)(MAX - MIN));

      // set the leds
      for(auto i = 0u; i < NUM_LEDS; i++) {
        led_strip.set_hsv(i, hue / 360.0f, 1.0, BRIGHTNESS);
      }

      sleep_ms(500);
    }
  }
  else {
    printf("BME280 not found :'(\n");
  }

  return 0;
}
