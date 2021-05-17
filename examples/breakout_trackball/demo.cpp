#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_trackball.hpp"

using namespace pimoroni;

static const uint8_t SENSITIVITY = 2;

// Trackball requires I2C at 100kHz or less.
I2C i2c(BOARD::BREAKOUT_GARDEN, 100000);
BreakoutTrackball trackball(&i2c);

int main() {
  stdio_init_all();

  if(!trackball.init()) {
    printf("Trackball failed to init!\n");
    return 1;
  }

  printf("Roll the Trackball to change colour!\n");

  trackball.set_rgbw(0, 0, 0, 64);

  while(true) {
    Trackball::State state = trackball.read();
    if(state.sw_pressed)
      trackball.set_rgbw(0, 0, 0, 255);
    else if(state.left > SENSITIVITY)
      trackball.set_rgbw(0, 0, 255, 0);
    else if(state.right > SENSITIVITY)
      trackball.set_rgbw(255, 0, 0, 0);
    else if(state.up > SENSITIVITY)
      trackball.set_rgbw(255, 255, 0, 0);
    else if(state.down > SENSITIVITY)
      trackball.set_rgbw(0, 255, 0, 0);
    else if(state.sw_changed)
      trackball.set_rgbw(0, 0, 0, 64);

    
    sleep_ms(20);
  }

  return 0;
}
