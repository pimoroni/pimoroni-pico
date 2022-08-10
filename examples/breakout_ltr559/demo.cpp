#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_ltr559.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutLTR559 ltr559(&i2c);

int main() {
  stdio_init_all();

  ltr559.init();

  uint8_t part_id = ltr559.part_id();
  if(part_id == LTR559_VALID_PART_ID) {
    printf("Found LTR559. Part ID: 0x%02x\n", part_id);
  }
  else
  {
    printf("Could not find LTR559. Got Part ID: 0x%02x, expected 0x%02x\n", part_id, LTR559_VALID_PART_ID);
  }

  while(true){
    bool new_data = ltr559.get_reading();
    if(new_data) {
      printf("Lux: %.2f Prox: %d\n", ltr559.data.lux, ltr559.data.proximity);
    }
    sleep_ms(100);
  };
  return 0;
}
