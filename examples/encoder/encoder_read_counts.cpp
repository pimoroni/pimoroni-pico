#include <cstdio>
#include "pico/stdlib.h"

#include "encoder.hpp"

/*
An example of how to read a mechanical rotary encoder.
*/

using namespace encoder;

// Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
const uint PIN_A = 26;  // The A channel pin
const uint PIN_B = 28;  // The B channel pin
const uint PIN_C = 27;  // The common pin
Encoder enc(pio0, 0, {PIN_A, PIN_B}, PIN_C);


int main() {
  stdio_init_all();

  // Uncomment the below line to reverse the counting direction
  // enc.direction(REVERSED_DIR);

  // Initialise the encoder
  enc.init();

  // Loop forever
  while(true) {
    // Print out the count, delta, step, and turn
    printf("Count = %ld, ", enc.count());
    printf("Delta = %ld, ", enc.delta());
    printf("Step = %d, ", enc.step());
    printf("Turn = %d\n", enc.turn());

    sleep_ms(100);
  }
}
