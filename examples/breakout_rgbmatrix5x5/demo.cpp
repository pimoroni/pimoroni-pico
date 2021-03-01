#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "breakout_rgbmatrix5x5.hpp"

using namespace pimoroni;

static const uint8_t NUM_COLORS = 4;


BreakoutRGBMatrix5x5 rgbmatrix5x5;

const pimoroni::RGBLookup colors[NUM_COLORS] = {
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {128, 128, 128}
};

int main() {
  rgbmatrix5x5.init();

  int x = 0;
  int y = 0;
  uint8_t col = 0;

  while(1) {
    pimoroni::RGBLookup color = colors[col];
    rgbmatrix5x5.set_pixel(x, y, color.r, color.g, color.b);
    rgbmatrix5x5.update();

    x++;
    if(x >= BreakoutRGBMatrix5x5::WIDTH) {
      x = 0;
      y++;
      if(y >= BreakoutRGBMatrix5x5::HEIGHT) {
        y = 0;
        col++;
        if(col >= NUM_COLORS)
          col = 0;
        sleep_ms(500);
      }
    }
    sleep_ms(50);
  }


  return 0;
}
