#include <stdio.h>
#include "pico/stdlib.h"

// Uncomment the below line to switch from the PMW3901 to the PAA5100
//#define USE_PAA5100

#ifndef USE_PAA5100
  #include "breakout_pmw3901.hpp"
#else
  #include "breakout_paa5100.hpp"
#endif

using namespace pimoroni;

#ifndef USE_PAA5100
  typedef BreakoutPMW3901 FlowSensor;
#else
  typedef BreakoutPAA5100 FlowSensor;
#endif

FlowSensor flo(BG_SPI_FRONT);

int main() {
  stdio_init_all();

  flo.init();
  flo.set_rotation(FlowSensor::DEGREES_0);
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
