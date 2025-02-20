#include "pico/stdlib.h"

#include "inventor.hpp"

/*
An example of applying a wave pattern to a group of servos and the LEDs.

Press "User" to exit the program.
*/

using namespace inventor;

// The speed that the LEDs will cycle at
const uint SPEED = 5;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4;

// How many times to update LEDs and Servos per second
const uint UPDATES = 50;

// How far from zero to move the servos
constexpr float SERVO_EXTENT = 60.0f;


// Create a new Inventor object
Inventor board;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    float offset = 0.0f;

    // Make rainbows until the user button is pressed
    while(!board.switch_pressed()) {

      offset += (float)SPEED / 1000.0f;

      // Update all the Servos
      for(auto i = 0u; i < NUM_SERVOS; i++) {
          float angle = (((float)i / (float)NUM_SERVOS) + offset) * (float)M_PI;
          board.servos[i].value(sin(angle) * SERVO_EXTENT);

          // Read back the servo's angle and use that to set a hue on the neighbouring LED
          float hue = ((board.servos[i].value() / SERVO_EXTENT) + 1) * 0.333f;
          board.leds.set_hsv(i + LED_SERVO_1, hue, 1.0f, BRIGHTNESS);
      }

      sleep_ms(1000 / UPDATES);
    }

    // Stop all the servos
    for(auto i = 0u; i < NUM_SERVOS; i++) {
      board.servos[i].disable();
    }

    // Turn off the LED bars
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}
