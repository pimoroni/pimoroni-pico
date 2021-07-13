#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "bh1745.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BH1745 bh1745(&i2c, BH1745::DEFAULT_I2C_ADDRESS);

int main() {
  stdio_init_all();

  if (!bh1745.init()) {
      printf("Failed to set up sensor\n");
      return 1;
  }

  uint8_t chip_id = bh1745.get_chip_id();
  printf("Found BH1745. Chip ID: 0x%02x\n", chip_id);

  bh1745.set_leds(true);

  while(true) {
    rgbc_t colour = bh1745.get_rgbc_raw();
    printf("Colour: %d %d %d %d\n", colour.r, colour.g, colour.b, colour.c);
    colour = bh1745.get_rgb_clamped();
    printf("Clamped: %d %d %d %d\n", colour.r, colour.g, colour.b, colour.c);
    colour = bh1745.get_rgb_scaled();
    printf("Scaled: %d %d %d %d\n\n", colour.r, colour.g, colour.b, colour.c);
    sleep_ms(1000);
  }

  return 0;
}