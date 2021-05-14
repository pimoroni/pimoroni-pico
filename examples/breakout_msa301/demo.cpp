#include <stdio.h>
#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

#include "breakout_msa301.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutMSA301 msa301(&i2c);

int main() {
  stdio_init_all();

  msa301.init();

  uint8_t part_id = msa301.part_id();
  printf("Found MSA301. Part ID: 0x%02x\n", part_id);

  msa301.enable_interrupts(MSA301::FREEFALL | MSA301::ORIENTATION);

  while(true){
    printf("X: %f, Y: %f, Z: %f", msa301.get_x_axis(), msa301.get_y_axis(), msa301.get_z_axis());
    printf(", Freefall? %d", msa301.read_interrupt(MSA301::FREEFALL));
    printf(", Orientation: %d\n", msa301.get_orientation());
    sleep_ms(100);
  };
  return 0;
}
