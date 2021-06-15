#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_paa5100.hpp"

using namespace pimoroni;

BreakoutPAA5100 flo(BG_SPI_FRONT);

int main() {
  stdio_init_all();

  flo.init();
  flo.set_rotation(BreakoutPAA5100::DEGREES_0);
  int16_t tx = 0, ty = 0;
  int16_t x = 0, y = 0;
  while(true) {
    if(flo.get_motion(x, y)) {
      tx += x;
      ty += y;
      printf("Relative: x %6d, y %6d | Absolute: tx %6d, ty %6d\n", x, y, tx, ty);
    }
    sleep_ms(10);
  };
  return 0;
}
