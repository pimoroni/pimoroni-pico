#include <cstdio>
#include "pico/stdlib.h"

#include "automation.hpp"

/*
Shows how to read the 3 ADC terminals of Automation 2040 W.

Press "A" to exit the program.
*/

//#define AUTOMATION_MINI // Uncomment for Automation Mini

using namespace automation;

// How many times to update per second
const uint UPDATES = 10;

// The friendly names to give each ADC input
const char* ADC_NAMES[] = {"A1", "A2", "A3"};


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

    // Read the ADCs until user switch is pressed
    while(!board.switch_pressed(SWITCH_A)) {

      // Read each ADC in turn and print its voltage
      for(uint i = 0; i < board.NUM_ADCS; i++) {
        float voltage = board.read_adc(i);
        printf("%s = %f, ", ADC_NAMES[i], voltage);
      }

      // Print a new line
      printf("\n");

      sleep_ms(1000 / UPDATES);
    }

    // Put the board back into a safe state
    board.reset();
  }
}
