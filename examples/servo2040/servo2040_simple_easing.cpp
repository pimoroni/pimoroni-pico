#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "button.hpp"

/*
An example of how to move a servo smoothly between random positions.

Press "Boot" to exit the program.
*/

using namespace servo;

// How many times to update Servos per second
const uint UPDATES = 50;

// The time to travel between each random value
const uint TIME_FOR_EACH_MOVE = 2;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How far from zero to move the servo
constexpr float SERVO_EXTENT = 80.0f;

// Whether or not to use a cosine path between values
const bool USE_COSINE = true;


// Create the user button
Button user_sw = Button(servo2040::USER_SW);

// Create a servo on pin 0
Servo s = Servo(servo2040::SERVO_1);


int main() {
  stdio_init_all();

  // Initialise the servo
  s.init();

  // Get the initial value and create a random end value between the extents
  float start_value = s.get_mid_value();
  float end_value = (((float)rand() / (float)RAND_MAX) * (SERVO_EXTENT * 2.0f)) - SERVO_EXTENT;

  uint update = 0;

  // Continually move the servo until the user button is pressed
  while(!user_sw.raw()) {

    // Calculate how far along this movement to be
    float percent_along = (float)update / (float)UPDATES_PER_MOVE;

    if(USE_COSINE) {
      // Move the servo between values using cosine
      s.to_percent(cos(percent_along * (float)M_PI), 1.0, -1.0, start_value, end_value);
    }
    else {
      // Move the servo linearly between values
      s.to_percent(percent_along, 0.0, 1.0, start_value, end_value);
    }

    // Print out the value the servo is now at
    printf("Value = %f\n", s.get_value());

    // Move along in time
    update++;

    // Have we reached the end of this movement?
    if(update >= UPDATES_PER_MOVE) {
      // Reset the counter
      update = 0;

      // Set the start as the last end and create a new random end value
      start_value = end_value;
      end_value = (((float)rand() / (float)RAND_MAX) * (SERVO_EXTENT * 2.0f)) - SERVO_EXTENT;
    }

    sleep_ms(1000 / UPDATES);
  }

  // Disable the servo
  s.disable();
}
