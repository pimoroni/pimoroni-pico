#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "bh1745.hpp"

using namespace pimoroni;

BH1745 bh1745(0x39);

int main() {
  setup_default_uart();

  if (bh1745.init() == 1) {
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