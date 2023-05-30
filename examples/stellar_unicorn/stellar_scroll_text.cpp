#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "stellar_unicorn.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(16, 16, nullptr);
StellarUnicorn stellar_unicorn;

std::string message = "Pirate. Monkey. Robot. Ninja.";

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

  stdio_init_all();

  stellar_unicorn.init();

  float scroll = -16.0f;

  while(true) {
    //uint time_ms = to_ms_since_boot(get_absolute_time());

    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_UP)) {
      stellar_unicorn.adjust_brightness(+0.01);
    }
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      stellar_unicorn.adjust_brightness(-0.01);
    }

    int width = graphics.measure_text(message, 1);
    scroll += 0.125f;

    if(scroll > width) {
      scroll = -16.0f;
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();

    uint8_t r = 0, g = 0, b = 0;
    from_hsv(scroll / 100.0f, 1.0f, 0.5f, r, g, b);
    graphics.set_pen(r, g, b);
    graphics.text(message, Point(0 - scroll, 5), -1, 0.55);
   
    stellar_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
