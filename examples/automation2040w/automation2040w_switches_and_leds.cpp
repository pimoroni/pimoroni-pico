#include <cstdio>
#include "pico/stdlib.h"

#include "automation.hpp"

/*
An example of the user switches and LEDs on Automation 2040 W.

Press "A" and "B" together to exit the program.
*/

//#define AUTOMATION_MINI // Uncomment for Automation Mini

using namespace automation;

// How many times to update per second
const uint UPDATES = 10;

// The friendly names to give each user input
const char* SWITCH_NAMES[] = {"SW_A", "SW_B"};

// How much to change a LED's brightess each update
const uint LED_INCREMENT = 2;


// Create a new Automation2040W
Automation2040W board;


int main() {
  stdio_init_all();

  #ifndef AUTOMATION_MINI
  // Attempt to initialise the board
  if(board.init(false)) { 
  
  #else 
  
  if(board.init(true)) { 
  
  #endif

    // Enable the Conn LED
    board.conn_led(true);  // Full Brightness

    float led_brightnesses[] = {0.0f, 0.0f};

    // Interact with the switches and LEDs until both are pressed simultaneously
    while(!board.switch_pressed(SWITCH_A) || !board.switch_pressed(SWITCH_B)) {

      for(uint i = 0; i < board.NUM_SWITCHES; i++) {
        // Change the LED brightness based on switch's state
        if(board.switch_pressed(i)) {
          printf("%s = Pressed, ", SWITCH_NAMES[i]);
          led_brightnesses[i] = MIN(led_brightnesses[i] + LED_INCREMENT, 100.0f);
        }
        else {
          printf("%s = Released, ", SWITCH_NAMES[i]);
          led_brightnesses[i] = MAX(led_brightnesses[i] - LED_INCREMENT, 0.0f);
        }

        // Apply the new brightnesses to the LEDs
        board.switch_led(i, led_brightnesses[i]);
      }

      // Print a new line
      printf("\n");

      sleep_ms(1000 / UPDATES);
    }

    // Put the board back into a safe state
    board.reset();
  }
}
