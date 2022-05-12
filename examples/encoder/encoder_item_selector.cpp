#include <cstdio>
#include "pico/stdlib.h"

#include "encoder.hpp"

/*
A demonstration of a rotary encoder being used to
select items based on its physical position.

This requires that the encoder is positioned in the same
direction at the start of every program run (e.g. upwards).
"""
*/

using namespace encoder;

// Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
const uint PIN_A = 26;  // The A channel pin
const uint PIN_B = 28;  // The B channel pin
const uint PIN_C = 27;  // The common pin
Encoder enc(pio0, 0, {PIN_A, PIN_B}, PIN_C);


// An array of items, up to the encoder's counts_per_rev
const char* ITEMS[] = {"Red", "Orange", "Yellow", "Green", "Blue", "Indigo", "Violet", "Black", "White"};
const int16_t NUM_ITEMS = count_of(ITEMS);

int16_t last_step = -1;


int main() {
  stdio_init_all();

  // Sleep 8 seconds to give enough time to connect up a terminal
  sleep_ms(8000);

  // Uncomment the below line to reverse the counting direction
  // enc.direction(REVERSED_DIR);

  // Initialise the encoder
  enc.init();

  // Loop forever
  while(true) {
    int16_t step = enc.step();
    if(step != last_step) {
      if(step < NUM_ITEMS)
        printf("%d/%d: %s\n", step, (int)enc.counts_per_rev() - 1, ITEMS[step]);
      else
        printf("%d/%d: %s\n", step, (int)enc.counts_per_rev() - 1, "Undefined");
      last_step = step;
    }
  }
}
