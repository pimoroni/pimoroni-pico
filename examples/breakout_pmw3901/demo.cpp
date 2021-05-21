#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_pmw3901.hpp"

using namespace pimoroni;

BreakoutPMW3901 flo;

int main() {
  stdio_init_all();

  sleep_ms(10000);

  flo.init();
  flo.set_rotation(BreakoutPMW3901::DEGREES_0);

  //uint8_t tx = 0, ty = 0;

  int16_t x = 0, y = 0;
  while(true) {
    flo.get_motion(x, y);
    printf("tick\n");
    //tx;
    sleep_ms(1000);
  };
  return 0;
}
