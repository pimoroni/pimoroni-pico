#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"

/*
Demonstrates how to read the angles of Motor 2040's four encoders.

Press "Boot" to exit the program.
*/

using namespace motor;
using namespace encoder;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The counts per revolution of the motor's output shaft
constexpr float COUNTS_PER_REV = MMME_CPR * GEAR_RATIO;


// Create an array of encoder pointers
const pin_pair encoder_pins[] = {motor2040::ENCODER_A, motor2040::ENCODER_B,
                                 motor2040::ENCODER_C, motor2040::ENCODER_D};
const char* ENCODER_NAMES[] = {"A", "B", "C", "D"};
const uint NUM_ENCODERS = count_of(encoder_pins);
Encoder *encoders[NUM_ENCODERS];

// Create the user button
Button user_sw(motor2040::USER_SW);


int main() {
  stdio_init_all();

  // Fill the array of motors, and initialise them. Up to 8 motors can be created
  for(auto e = 0u; e < NUM_ENCODERS; e++) {
    encoders[e] = new Encoder(pio0, e, encoder_pins[e], PIN_UNUSED, NORMAL_DIR, COUNTS_PER_REV, true);
    encoders[e]->init();
  }

  // Uncomment the below lines to reverse
  // the counting direction of an encoder
  // encoders[0].direction(REVERSED_DIR);
  // encoders[1].direction(REVERSED_DIR);
  // encoders[2].direction(REVERSED_DIR);
  // encoders[3].direction(REVERSED_DIR);

  // Read the encoders until the user button is pressed
  while(!user_sw.raw()) {
    // Print out the angle of each encoder
    for(auto e = 0u; e < NUM_ENCODERS; e++) {
      printf("%s = %f, ", ENCODER_NAMES[e], encoders[e]->degrees());
    }
    printf("\n");

    sleep_ms(100);
  }
}
