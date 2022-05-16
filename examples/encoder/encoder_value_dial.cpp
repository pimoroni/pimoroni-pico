#include <cstdio>
#include "pico/stdlib.h"

#include "encoder.hpp"

/*
A demonstration of a rotary encoder being used to control a value.
*/

using namespace encoder;

// Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
const uint PIN_A = 26;  // The A channel pin
const uint PIN_B = 28;  // The B channel pin
const uint PIN_C = 27;  // The common pin
Encoder enc(pio0, 0, {PIN_A, PIN_B}, PIN_C);


// The min and max value
const int MIN_VALUE = 0;
const int MAX_VALUE = 100;

int value = 50;


int main() {
  stdio_init_all();

  // Sleep 8 seconds to give enough time to connect up a terminal
  sleep_ms(8000);

  // Uncomment the below line to reverse the counting direction
  // enc.direction(REVERSED_DIR);

  // Initialise the encoder
  enc.init();

  // Print out the initial value
  printf("Value = %d\n", value);

  // Loop forever
  while(true) {
    int32_t delta = enc.delta();
    if(delta != 0) {
      if(delta > 0)
        value = MIN(value + 1, MAX_VALUE);
      else
        value = MAX(value - 1, MIN_VALUE);

      // Print out the new value
      printf("Value = %d\n", value);
    }
  }
}
