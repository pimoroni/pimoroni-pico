#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to read the angles of Inventor 2040 W's two encoders.

Press "User" to exit the program.
*/

using namespace inventor;

// Create an array of encoder pointers
const char* ENCODER_NAMES[] = {"A", "B"};


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

    // Attempt to initialise the board
  if(board.init()) {

    // Uncomment the below lines to reverse
    // the counting direction of an encoder
    // board.encoders[MOTOR_A].direction(REVERSED_DIR);
    // board.encoders[MOTOR_B].direction(REVERSED_DIR);

    // Read the encoders until the user button is pressed
    while(!board.switch_pressed()) {
      // Print out the angle of each encoder
      for(auto e = 0u; e < NUM_MOTORS; e++) {
        printf("%s = %f, ", ENCODER_NAMES[e], board.encoders[e].degrees());
      }
      printf("\n");

      sleep_ms(100);
    }
  }
}
