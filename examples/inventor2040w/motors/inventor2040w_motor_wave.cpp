#include "pico/stdlib.h"

#include "inventor.hpp"

/*
An example of applying a wave pattern to Inventor 2040 W's motors and LEDs.

Press "User" to exit the program.
*/

using namespace inventor;

// The speed that the LEDs and motors will cycle at
const uint SPEED = 5;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4;

// How many times to update LEDs and motors per second
const uint UPDATES = 50;

// How far from zero to drive the motors
constexpr float SPEED_EXTENT = 1.0f;

const uint HALF_LEDS = NUM_LEDS / 2;


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

    // Attempt to initialise the board
  if(board.init()) {

    float offset = 0.0f;

    // Make rainbows until the user button is pressed
    while(!board.switch_pressed()) {

      offset += (float)SPEED / 1000.0f;

      // Update the LED bars
      for(auto i = 0u; i < HALF_LEDS; i++) {
        float hue = (float)i / (float)(NUM_LEDS * 4);
        board.leds.set_hsv(i, hue + offset, 1.0, BRIGHTNESS);
        board.leds.set_hsv(NUM_LEDS - i - 1, hue + offset, 1.0, BRIGHTNESS);
      }

      // Update both motors
      for(auto m = 0u; m < NUM_MOTORS; m++) {
        float angle = ((float)m + offset) * (float)M_PI;
        board.motors[m].speed(sin(angle) * SPEED_EXTENT);
      }

      sleep_ms(1000 / UPDATES);
    }

    // Stop both motors
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].disable();
    }

    // Turn off the LEDs
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}
