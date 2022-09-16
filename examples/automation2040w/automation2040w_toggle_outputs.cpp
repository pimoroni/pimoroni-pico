#include <cstdio>
#include "pico/stdlib.h"

#include "automation.hpp"

/*
Demonstrates how to toggle each of Automation 2040 W's output terminals.

Press "A" to exit the program.
*/

//#define AUTOMATION_MINI // Uncomment for Automation Mini

using namespace automation;

// How much time to wait between each toggle (in milliseconds)
const uint TIME_PER_TOGGLE_MS = 500;

// The friendly names to give each digital output
const char* OUTPUT_NAMES[] = {"O1", "O2", "O3"};


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

    // Toggle the outputs until the user switch is pressed
    while(!board.switch_pressed(SWITCH_A)) {

      // Toggle an output
      board.output(index, toggle);

      // Print the state of all outputs
      for(uint i = 0; i < board.NUM_OUTPUTS; i++) {
        printf("%s = %d, ", OUTPUT_NAMES[i], (int)board.output(i));
      }

      // Print a new line
      printf("\n");

      index++;              // Move on to the next output
      if(index >= board.NUM_OUTPUTS) {
        index = 0;          // Go back to the first output
        toggle = !toggle;   // Invert the toggle value
      }

      sleep_ms(TIME_PER_TOGGLE_MS);
    }

    // Put the board back into a safe state
    board.reset();
  }
}
