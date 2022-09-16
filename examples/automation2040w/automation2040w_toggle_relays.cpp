#include <cstdio>
#include "pico/stdlib.h"

#include "automation.hpp"

/*
Demonstrates how to toggle the actuation state of each of Automation 2040 W's relays.

Press "A" to exit the program.
*/

//#define AUTOMATION_MINI // Uncomment for Automation Mini

using namespace automation;

// How much time to wait between each toggle (in milliseconds)
const uint TIME_PER_TOGGLE_MS = 500;

// The friendly names to give each relay
const char* RELAY_NAMES[] = {"R1", "R2", "R3"};


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



    // Enable the LED of the switch used to exit the loop
    board.switch_led(SWITCH_A, 50.0f);  // Half Brightness

    bool toggle = true;
    uint index = 0;

    // Toggle the relays until the user switch is pressed
    while(!board.switch_pressed(SWITCH_A)) {

      // Toggle a relay
      board.relay(index, toggle);

      // Print the state of all relays
      for(uint i = 0; i < board.NUM_RELAYS; i++) {
        printf("%s = %d, ", RELAY_NAMES[i], (int)board.relay(i));
      }

      // Print a new line
      printf("\n");

      index++;              // Move on to the next relay
      if(index >= board.NUM_RELAYS) {
        index = 0;          // Go back to the first relay
        toggle = !toggle;   // Invert the toggle value
      }

      sleep_ms(TIME_PER_TOGGLE_MS);
    }

    // Put the board back into a safe state
    board.reset();
  }
}
