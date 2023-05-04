#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
A demonstration of reading the rotary dial of the Encoder Wheel breakout.
*/

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
int position = 0;
float hue = 0.0f;


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {

    // Set the first LED
    wheel.clear();
    wheel.set_hsv(position, hue, 1.0f, 1.0f);
    wheel.show();

    // Loop forever
    while(true) {
      // Has the dial been turned since the last time we checked?
      int16_t change = wheel.delta();
      if(change != 0) {
        // Print out the direction the dial was turned, and the count
        if(change > 0)
            printf("Clockwise, Count = %d\n", wheel.count());
        else
            printf("Counter Clockwise, Count = %d\n", wheel.count());

        // Record the new position (from 0 to 23)
        position = wheel.step();

        // Record a colour hue from 0.0 to 1.0
        hue = fmodf(wheel.revolutions(), 1.0f);

        // Set the LED at the new position to the new hue
        wheel.clear();
        wheel.set_hsv(position, hue, 1.0f, 1.0f);
        wheel.show();
      }
    }
  }

  return 0;
}