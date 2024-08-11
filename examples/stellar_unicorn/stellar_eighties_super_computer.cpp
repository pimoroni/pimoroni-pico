#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "stellar_unicorn.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(16, 16, nullptr);
StellarUnicorn stellar_unicorn;

float lifetime[16][16];
float age[16][16];

int main() {

  stdio_init_all();

  for(int y = 0; y < 16; y++) {
    for(int x = 0; x < 16; x++) {
      lifetime[x][y] = 1.0f + ((rand() % 10) / 100.0f);
      age[x][y] = ((rand() % 100) / 100.0f) * lifetime[x][y];
    }
  }

  stellar_unicorn.init();

  while(true) {
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_UP)) {
      stellar_unicorn.adjust_brightness(+0.01);
    }
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      stellar_unicorn.adjust_brightness(-0.01);
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();

    for(int y = 0; y < 16; y++) {
      for(int x = 0; x < 16; x++) {
        if(age[x][y] < lifetime[x][y] * 0.3f) {
          graphics.set_pen(230, 150, 0);
          graphics.pixel(Point(x, y));
        }else if(age[x][y] < lifetime[x][y] * 0.5f) {
          float decay = (lifetime[x][y] * 0.5f - age[x][y]) * 5.0f;
          graphics.set_pen(decay * 230, decay * 150, 0);
          graphics.pixel(Point(x, y));
        }

        if(age[x][y] >= lifetime[x][y]) {
          age[x][y] = 0.0f;
          lifetime[x][y] = 1.0f + ((rand() % 10) / 100.0f);
        }

        age[x][y] += 0.01f;
      }
    }

    stellar_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
