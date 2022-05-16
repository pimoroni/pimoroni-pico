#include <cstdio>
#include "pico/stdlib.h"

#include "encoder.hpp"

/*
An example of how to read a mechanical rotary encoder, only when a change has occurred.
*/

using namespace encoder;

// Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
const uint PIN_A = 26;  // The A channel pin
const uint PIN_B = 28;  // The B channel pin
const uint PIN_C = 27;  // The common pin
Encoder enc(pio0, 0, {PIN_A, PIN_B}, PIN_C);


int main() {
  stdio_init_all();

  // Sleep 8 seconds to give enough time to connect up a terminal
  sleep_ms(8000);

  // Uncomment the below line to reverse the counting direction
  // enc.direction(REVERSED_DIR);

  // Initialise the encoder
  enc.init();

  // Print out the initial count, step, and turn (they should all be zero)
  printf("Count = %ld, ", enc.count());
  printf("Step = %d, ", enc.step());
  printf("Turn = %d\n", enc.turn());

  // Loop forever
  while(true) {
    if(enc.delta() != 0) {
      // Print out the new count, step, and turn
      printf("Count = %ld, ", enc.count());
      printf("Step = %d, ", enc.step());
      printf("Turn = %d\n", enc.turn());
    }
  }
}
