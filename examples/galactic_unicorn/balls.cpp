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

// extra row of pixels for sourcing flames and averaging
float heat[53][13] = {0.0f};

int main() {

  stdio_init_all();

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

    
    for(int y = 0; y < 12; y++) {
      for(int x = 0; x < 53; x++) {
        if(heat[x][y] > 0.5f) {
          graphics.set_pen(255, 255, 180);
          graphics.pixel(Point(x, y));
        }else if(heat[x][y] > 0.4f) {
          graphics.set_pen(220, 160, 0);
          graphics.pixel(Point(x, y));
        }else if(heat[x][y] > 0.3f) {
          graphics.set_pen(180, 50, 0);
          graphics.pixel(Point(x, y));
        }else if(heat[x][y] > 0.2f) {
          graphics.set_pen(40, 40, 40);
          graphics.pixel(Point(x, y));
        }

        // update this pixel by averaging the below pixels
        if(x == 0) {
          heat[x][y] = (heat[x][y] + heat[x][y + 2] + heat[x][y + 1] + heat[x + 1][y + 1]) / 4.0f;
        } else if(x == 52) {
          heat[x][y] = (heat[x][y] + heat[x][y + 2] + heat[x][y + 1] + heat[x - 1][y + 1]) / 4.0f;
        } else {
          heat[x][y] = (heat[x][y] + heat[x][y + 2] + heat[x][y + 1] + heat[x - 1][y + 1] + heat[x + 1][y + 1]) / 5.0f;
        } 

        heat[x][y] -= 0.01f;
        heat[x][y] = heat[x][y] < 0.0f ? 0.0f: heat[x][y];
      }
    }

    galactic_unicorn.update(graphics);

    // clear the bottom row and then add a new fire seed to it
    for(int x = 0; x < 53; x++) {
      heat[x][11] = 0.0f;
    }

    // add a new random heat source
    for(int c = 0; c < 5; c++) {
      int px = (rand() % 51) + 1;
      heat[px][11] = 1.0f;
      heat[px + 1][11] = 1.0f;
      heat[px - 1][11] = 1.0f;
      heat[px][12] = 1.0f;
      heat[px + 1][12] = 1.0f;
      heat[px - 1][12] = 1.0f;
    }
    
    sleep_ms(50);
  }

  return 0;
}
