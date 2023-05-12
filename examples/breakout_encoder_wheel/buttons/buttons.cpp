#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
A demonstration of reading the 5 buttons on Encoder Wheel.
*/

// Constants
const std::string BUTTON_NAMES[] = {"Up", "Down", "Left", "Right", "Centre"};

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
bool last_pressed[NUM_BUTTONS] = {false, false, false, false, false};
bool pressed[NUM_BUTTONS] = {false, false, false, false, false};


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Loop forever
    while(true) {
      // Read all of the encoder wheel's buttons
      for(int b = 0 ; b < NUM_BUTTONS; b++) {
        pressed[b] = wheel.pressed(b);
        if(pressed[b] != last_pressed[b]) {
          printf("%s %s\n", BUTTON_NAMES[b].c_str(), pressed[b] ? "Pressed" : "Released");
        }
        last_pressed[b] = pressed[b];
      }

      // Clear the LED ring
      wheel.clear();

      for(int i = 0; i < NUM_LEDS; i++) {
        if(i % 6 == 3) {
          wheel.set_rgb(i, 64, 64, 64);
        }
      }

      // If up is pressed, set the top LEDs to yellow
      if(pressed[UP]) {
          int mid = NUM_LEDS;
          for(int i = mid - 2; i < mid + 3; i++) {
            wheel.set_rgb(i % NUM_LEDS, 255, 255, 0);
          }
      }

      // If right is pressed, set the right LEDs to red
      if(pressed[RIGHT]) {
          int mid = NUM_LEDS / 4;
          for(int i = mid - 2; i < mid + 3; i++) {
            wheel.set_rgb(i % NUM_LEDS, 255, 0, 0);
          }
      }

      // If down is pressed, set the bottom LEDs to green
      if(pressed[DOWN]) {
          int mid = NUM_LEDS / 2;
          for(int i = mid - 2; i < mid + 3; i++) {
            wheel.set_rgb(i % NUM_LEDS, 0, 255, 0);
          }
      }

      // If left is pressed, set the left LEDs to blue
      if(pressed[LEFT]) {
          int mid = (NUM_LEDS * 3) / 4;
          for(int i = mid - 2; i < mid + 3; i++) {
            wheel.set_rgb(i % NUM_LEDS, 0, 0, 255);
          }
      }

      // If centre is pressed, set the diagonal LEDs to half white
      if(pressed[CENTRE]) {
        for(int i = 0; i < NUM_LEDS; i++) {
          if(i % 6 >= 2 && i % 6 <= 4) {
            wheel.set_rgb(i, 128, 128, 128);
          }
        }
      }
      wheel.show();
    }
  }

  return 0;
}