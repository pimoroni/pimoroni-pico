#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
How to read the buttons and rotary dial of the Encoder Wheel breakout, only when an interrupt occurs.
*/

// Constants
const std::string BUTTON_NAMES[] = {"Up", "Down", "Left", "Right", "Centre"};

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c,
                           BreakoutEncoderWheel::DEFAULT_IOE_I2C_ADDRESS,
                           BreakoutEncoderWheel::DEFAULT_LED_I2C_ADDRESS,
                           3);  // 3 for BG_BASE, 22 for EXPLORER_BASE, or 19 for some RP2040 boards
// If wiring the breakout via the qw/st connector, use the below line instead
// BreakoutEncoderWheel wheel(&i2c);

// Variables
bool last_pressed[NUM_BUTTONS] = {false, false, false, false, false};
bool pressed[NUM_BUTTONS] = {false, false, false, false, false};
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

    // Clear any left over interrupt from previous code
    wheel.clear_interrupt_flag();

    // Loop forever
    while(true) {

      // Check if the interrupt has fired
      if(wheel.get_interrupt_flag()) {
        wheel.clear_interrupt_flag();

        // Read all of the encoder wheel's buttons
        for(int b = 0 ; b < NUM_BUTTONS; b++) {
          pressed[b] = wheel.pressed(b);
          if(pressed[b] != last_pressed[b]) {
            printf("%s %s\n", BUTTON_NAMES[b].c_str(), pressed[b] ? "Pressed" : "Released");
          }
          last_pressed[b] = pressed[b];
        }

        // The interrupt may have come from several sources,
        // so check if it was a position change
        int new_position = wheel.step();
        if(new_position != position) {
          // Record the new position (from 0 to 23)
          position = new_position;
          printf("Position = %d\n", position);

          // Record a colour hue from 0.0 to 1.0
          hue = fmodf(wheel.revolutions(), 1.0f);

          // Set the LED at the new position to the new hue
          wheel.clear();
          wheel.set_hsv(position, hue, 1.0f, 1.0f);
          wheel.show();
        }
      }
    }
  }

  return 0;
}