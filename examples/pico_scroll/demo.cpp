#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_scroll.hpp"

using namespace pimoroni;

PicoScroll pico_scroll;

int main() {

  pico_scroll.init();

  uint32_t i = 0;
  while(true) {
    i++;

    pico_scroll.clear();

    for(int y = 0; y < 7; y++) {
      for(int x = 0; x < 17; x++) {
        int v = (x + y + (i / 25)) % 2 == 0 ? 0 : 3;
        pico_scroll.set_pixel(x, y, v);
      }
    }

    pico_scroll.update();

    sleep_ms(10);
  }

  return 0;
}
