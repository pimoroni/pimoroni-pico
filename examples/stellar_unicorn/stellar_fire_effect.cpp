#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "stellar_unicorn.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(16, 16, nullptr);
StellarUnicorn stellar_unicorn;

// extra row of pixels for sourcing flames and averaging
int width = 16;
int height = 17;

// a buffer that's at least big enough to store 55 x 15 values (to allow for both orientations)
float heat[2000] = {0.0f};

void set(int x, int y, float v) {
  heat[x + y * width] = v;
}

float get(int x, int y) {
  /*if(x < 0 || x >= width || y < 0 || y >= height) {
    return 0.0f;
  }*/
  x = x < 0 ? 0 : x;
  x = x >= width ? width - 1 : x;

  return heat[x + y * width];
}

int main() {

  stdio_init_all();

  stellar_unicorn.init();
  stellar_unicorn.set_brightness(0.2);

  bool landscape = true;
/*
  while(true) {
    stellar_unicorn.set_pixel(0, 0, 255, 0, 0);
    stellar_unicorn.set_pixel(1, 1, 0, 255, 0);
    stellar_unicorn.set_pixel(2, 2, 0, 0, 255);
  }*/

  while(true) {
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_UP)) {
      stellar_unicorn.adjust_brightness(+0.01);
    }
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      stellar_unicorn.adjust_brightness(-0.01);
    }


    for(int y = 0; y < height; y++) {
      for(int x = 0; x < width; x++) {
        float value = get(x, y);

        graphics.set_pen(0, 0, 0);
        if(value > 0.5f) {
          graphics.set_pen(255, 255, 180);
        }else if(value > 0.4f) {
          graphics.set_pen(220, 160, 0);
        }else if(value > 0.3f) {
          graphics.set_pen(180, 30, 0);
        }else if(value > 0.22f) {
          graphics.set_pen(20, 20, 20);
        }

        if(landscape) {
          graphics.pixel(Point(x, y));
        }else{
          graphics.pixel(Point(y, x));
        }

        // update this pixel by averaging the below pixels
        float average = (get(x, y) + get(x, y + 2) + get(x, y + 1) + get(x - 1, y + 1) + get(x + 1, y + 1)) / 5.0f;

        // damping factor to ensure flame tapers out towards the top of the displays
        average *= 0.95f;

        // update the heat map with our newly averaged value
        set(x, y, average);
      }
    }

    stellar_unicorn.update(&graphics);

    // clear the bottom row and then add a new fire seed to it
    for(int x = 0; x < width; x++) {
      set(x, height - 1, 0.0f);
    }

    // add a new random heat source
    int source_count = landscape ? 5 : 1;
    for(int c = 0; c < source_count; c++) {
      int px = (rand() % (width - 4)) + 2;
      set(px    , height - 2, 1.0f);
      set(px + 1, height - 2, 1.0f);
      set(px - 1, height - 2, 1.0f);
      set(px    , height - 1, 1.0f);
      set(px + 1, height - 1, 1.0f);
      set(px - 1, height - 1, 1.0f);
    }

    sleep_ms(20);
  }

  return 0;
}
