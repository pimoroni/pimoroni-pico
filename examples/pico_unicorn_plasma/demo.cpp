/*
Pico Unicorn Plasma Example
Written by Tim Kerby https://github.com/tkerby

Hardware: Raspberry Pi Pico with Pimoroni Unicorn Pico Display
Printf over USB 9600 8N1 (see makefile)

Based on Adafruit Arduino Example for 32x32 panel
Originally written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon for Adafruit Industries.
https://github.com/adafruit/RGB-matrix-Panel/blob/master/examples/plasma_32x32/plasma_32x32.ino

BSD License
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include <time.h>

#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

int main() {
  stdio_init_all();
  pico_unicorn.init();
  pico_unicorn.clear();

  int state=0;

  const int Tperiod=32000;
  // const int period=128;
  // const int period=64;
  const int period=32;
  // const int period=16;

  for(;;) {
    unsigned char x, y;

    state = (state + 1) % period;

    uint16_t xx = std::min(state,period-state); // triangle wave from 0 to period/2

    // printf("%d %d\n", state, xx);

    sleep_ms((Tperiod+period/2)/period);

    for(y=0; y<7; y++) {
      for(x=0; x<16; x++) {
        pico_unicorn.set_pixel(x, y, xx, uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint8_t(xx), uint8_t(0), uint8_t(0));
      }
    }
  }

  return 0;
}
