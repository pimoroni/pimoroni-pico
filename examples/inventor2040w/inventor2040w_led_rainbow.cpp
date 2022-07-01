#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Displays a rotating rainbow pattern on Inventor 2040 W's onboard LED bars.

Press "User" to exit the program.
*/

using namespace inventor;

// The speed that the LEDs will cycle at
const uint SPEED = 5;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// How many times the LEDs will be updated per second
const uint UPDATES = 50;


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

      // Update all the LEDs
      for(auto i = 0u; i < NUM_LEDS; i++) {
        float hue = (float)i / (float)NUM_LEDS;
        board.leds.set_hsv(i, hue + offset, 1.0f, BRIGHTNESS);
      }

      sleep_ms(1000 / UPDATES);
    }

    // Turn off the LED bars
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}