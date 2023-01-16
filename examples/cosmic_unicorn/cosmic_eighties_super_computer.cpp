#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "cosmic_unicorn.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(32, 32, nullptr);
CosmicUnicorn cosmic_unicorn;

float lifetime[32][32];
float age[32][32];

int main() {

  stdio_init_all();

  for(int y = 0; y < 32; y++) {
    for(int x = 0; x < 32; x++) {
      lifetime[x][y] = 1.0f + ((rand() % 10) / 100.0f);
      age[x][y] = ((rand() % 100) / 100.0f) * lifetime[x][y];
    }
  }

  cosmic_unicorn.init();

  while(true) {
    if(cosmic_unicorn.is_pressed(cosmic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      cosmic_unicorn.adjust_brightness(+0.01);
    }
    if(cosmic_unicorn.is_pressed(cosmic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      cosmic_unicorn.adjust_brightness(-0.01);
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();

    for(int y = 0; y < 32; y++) {
      for(int x = 0; x < 32; x++) {
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

    cosmic_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
