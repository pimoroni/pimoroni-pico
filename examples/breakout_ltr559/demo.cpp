#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_ltr559.hpp"

using namespace pimoroni;

I2C i2c(4, 5);
BreakoutLTR559 ltr559(&i2c);

int main() {
  stdio_init_all();

  ltr559.init();

  uint8_t part_id = ltr559.part_id();
  printf("Found LTR559. Part ID: 0x%02x\n", part_id);

  while(true){
    bool new_data = ltr559.get_reading();
    if(new_data) {
      printf("Lux: %d Prox: %d\n", ltr559.data.lux, ltr559.data.proximity);
    }
    sleep_ms(100);
  };
  return 0;
}
