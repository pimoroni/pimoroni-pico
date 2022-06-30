#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Shows how to initialise and read the 6 GPIO headers of Inventor 2040 W.

Press "User" to exit the program.
*/

using namespace inventor;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

const char* GPIO_NAMES[] = {"GP0", "GP1", "GP2", "A0", "A1", "A2"};


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Set up the GPIOs as inputs
    for(uint i = 0; i < NUM_GPIOS; i++) {
      uint pin = GPIOS[i];
      gpio_set_function(pin, GPIO_FUNC_SIO);
      gpio_set_dir(pin, GPIO_IN);
      gpio_pull_down(pin);
    }

    // Read the GPIOs until the user button is pressed
    while(!board.switch_pressed()) {

      // Read each GPIO in turn and print its value
      for(uint i = 0; i < NUM_GPIOS; i++) {
        bool value = gpio_get(GPIOS[i]);
        printf("%s = %d, ", GPIO_NAMES[i], value);

        // Set the neighbouring LED to a colour based on
        // the input, with Green for high and Blue for low
        if(value)
          board.leds.set_hsv(i, 0.333, 1.0, BRIGHTNESS);
        else
          board.leds.set_hsv(i, 0.666, 1.0, BRIGHTNESS);
      }

      // Print a new line
      printf("\n");

      sleep_ms(0.1);
    }

    // Turn off the LED bars
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}