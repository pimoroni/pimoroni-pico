#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "breakout_rgbmatrix5x5.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutRGBMatrix5x5 rgbmatrix5x5(&i2c);

int main() {
  rgbmatrix5x5.init();

  const pimoroni::RGBLookup colors[4] = {
    {255, 0, 0},
    {0, 255, 0},
    {0, 0, 255},
    {128, 128, 128}
  };

  uint8_t col = 0;
  while(1) {
    pimoroni::RGBLookup color = colors[col];
    for(auto x = 0u; x < 5; x++) {
      for(auto y = 0u; y < 5; y++) {
        rgbmatrix5x5.set_pixel(x, y, color.r, color.g, color.b);
      }
    }
    rgbmatrix5x5.update(0);
    sleep_ms(1000);
    col++;
    col %= 4;
  }
  return 0;
}
