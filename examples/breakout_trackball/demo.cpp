#include <stdio.h>
#include "pico/stdlib.h"

#include "breakout_trackball.hpp"

using namespace pimoroni;

static const uint8_t SENSITIVITY = 2;


BreakoutTrackball trackball;

int main() {
  trackball.init();

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
