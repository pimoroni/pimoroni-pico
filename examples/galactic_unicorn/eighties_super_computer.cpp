#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#include "okcolor.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(53, 11, nullptr);
GalacticUnicorn galactic_unicorn;

float lifetime[53][11];
float age[53][11];

int main() {

  stdio_init_all();

  for(int y = 0; y < 11; y++) {
    for(int x = 0; x < 53; x++) {
      lifetime[x][y] = 1.0f + ((rand() % 10) / 100.0f);
      age[x][y] = ((rand() % 100) / 100.0f) * lifetime[x][y];
    }
  }

  galactic_unicorn.init();

  while(true) {
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      galactic_unicorn.adjust_brightness(+0.01);
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      galactic_unicorn.adjust_brightness(-0.01);
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();

    for(int y = 0; y < 11; y++) {
      for(int x = 0; x < 53; x++) {
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

    galactic_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
