#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/hub75/hub75.hpp"
#include "okcolor.hpp"

using namespace pimoroni;


// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.

//If the display looks streaky or corrupted then uncomment one of the other initalisers

//Works with our 32x32 panels https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=35962488650 https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=19321740999
Hub75 hub75(32, 32, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(64, 32, nullptr, PANEL_GENERIC, false);

//and 64x32 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=42312764298
//Hub75 hub75(64, 32, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(128, 32, nullptr, PANEL_GENERIC, false);

//Works with our 64x64 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=3029531983882
//Hub75 hub75(64, 64, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(128, 64, nullptr, PANEL_GENERIC, false);


PicoGraphics_PenRGB888 graphics(hub75.width, hub75.height, nullptr);

// extra row of pixels for sourcing flames and averaging
int width = hub75.width;
int height = hub75.height + 2;

// a buffer that's at least big enough to store 55 x 15 values (to allow for both orientations)
float heat[4500] = {0.0f};

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

// Interrupt callback required function 
void __isr dma_complete() {
    hub75.dma_complete();
}


int main() {

  //stdio_init_all();

  //Start hub75 driver
  hub75.start(dma_complete);

  bool landscape = true;

  while(true) {

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
        average *= landscape ? 0.99f : 0.99f;

        // update the heat map with our newly averaged value
        set(x, y, average);
      }
    }

    hub75.update(&graphics);

    // clear the bottom row and then add a new fire seed to it
    for(int x = 0; x < width; x++) {
      set(x, height - 1, 0.0f);
    }

    // add a new random heat source
    int source_count = landscape ? 7 : 1;
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
