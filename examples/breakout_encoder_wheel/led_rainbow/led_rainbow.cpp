#include <math.h>
#include <string>
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
Displays a rotating rainbow pattern on Encoder Wheel's LED ring.
*/

// Constants
constexpr float SPEED = 5.0f;       // The speed that the LEDs will cycle at
constexpr float BRIGHTNESS = 1.0f;  // The brightness of the LEDs
const uint UPDATES = 50;            // How many times the LEDs will be updated per second
const uint UPDATE_RATE_US = 1000000 / UPDATES;

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
float offset = 0.0;


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Loop forever
    while(true) {

      // Record the start time of this loop
      absolute_time_t start_time = get_absolute_time();

      offset += SPEED / 1000.0f;

      // Update all the LEDs
      for(int i = 0; i < NUM_LEDS; i++) {
        float hue = (float)i / NUM_LEDS;
        wheel.set_hsv(i, hue + offset, 1.0, BRIGHTNESS);
      }
      wheel.show();

      // Sleep until the next update, accounting for how long the above operations took to perform
      sleep_until(delayed_by_us(start_time, UPDATE_RATE_US));
    }
  }

  return 0;
}