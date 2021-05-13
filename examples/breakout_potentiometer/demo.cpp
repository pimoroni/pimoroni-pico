#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

#include "breakout_potentiometer.hpp"

using namespace pimoroni;

BreakoutPotentiometer pot;
bool toggle = false;

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

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  stdio_init_all();

  int16_t count = 0;
  if(pot.init()) {
    printf("Potentiometer found...\n");

    //pot.set_direction(BreakoutPotentiometer::DIRECTION_CCW);    // Uncomment this to flip the direction

    while(true) {
      gpio_put(PICO_DEFAULT_LED_PIN, toggle);
      toggle = !toggle;

      float percent = pot.read();

      printf("Percent: %d\n", (int)(percent * 100));
      uint8_t r, g, b;
      from_hsv(percent, 1.0f, 1.0f, r, g, b);
      pot.set_led(r, g, b);

      sleep_ms(20);
    }
  }
  else {
    printf("Potentiometer not found :'(\n");
    gpio_put(PICO_DEFAULT_LED_PIN, true);
  }

  return 0;
}
