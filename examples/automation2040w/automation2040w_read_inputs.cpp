#include <cstdio>
#include "pico/stdlib.h"

#include "automation.hpp"

/*
Shows how to read the 3 Input terminals of Automation 2040 W.

Press "A" to exit the program.
*/

//#define AUTOMATION_MINI // Uncomment for Automation Mini

using namespace automation;

// How many times to update per second
const uint UPDATES = 10;

// The friendly names to give each digital input
const char* INPUT_NAMES[] = {"I1", "I2", "I3", "I4"};


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

    // Read the inputs until user switch is pressed
    while(!board.switch_pressed(SWITCH_A)) {

      // Read each input in turn and print its voltage
      for(uint i = 0; i < board.NUM_INPUTS; i++) {
        bool value = board.read_input(i);
        printf("%s = %d, ", INPUT_NAMES[i], (int)value);
      }

      // Print a new line
      printf("\n");

      sleep_ms(1000 / UPDATES);
    }

    // Put the board back into a safe state
    board.reset();
  }
}
