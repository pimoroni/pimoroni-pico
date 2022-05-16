#include <stdio.h>
#include "pico/stdlib.h"

#include "pico_motor_shim.hpp"

/*
An example of how to perform simple movements of a 2-wheeled driving robot.
*/

using namespace motor;

// The scaling to apply to each motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// The speed to drive the wheels at, from 0.0 to SPEED_SCALE
constexpr float DRIVING_SPEED = SPEED_SCALE;


// Create the left and right motors with a given speed scale
// Swap the numbers and directions if this is different to your setup
Motor left(pico_motor_shim::MOTOR_1, NORMAL_DIR, SPEED_SCALE);
Motor right(pico_motor_shim::MOTOR_2, REVERSED_DIR, SPEED_SCALE);


// Helper functions for driving in common directions
void forward(float speed=DRIVING_SPEED) {
  left.speed(speed);
  right.speed(speed);
}

void backward(float speed=DRIVING_SPEED) {
  left.speed(-speed);
  right.speed(-speed);
}

void turn_left(float speed=DRIVING_SPEED) {
  left.speed(-speed);
  right.speed(speed);
}

void turn_right(float speed=DRIVING_SPEED) {
  left.speed(speed);
  right.speed(-speed);
}

void curve_forward_left(float speed=DRIVING_SPEED) {
  left.speed(0.0);
  right.speed(speed);
}

void curve_forward_right(float speed=DRIVING_SPEED) {
  left.speed(speed);
  right.speed(0.0);
}

void curve_backward_left(float speed=DRIVING_SPEED) {
  left.speed(0.0);
  right.speed(-speed);
}

void curve_backward_right(float speed=DRIVING_SPEED) {
  left.speed(-speed);
  right.speed(0.0);
}

void stop() {
  left.stop();
  right.stop();
}

void coast() {
  left.coast();
  right.coast();
}


int main() {
  stdio_init_all();

  // Initialise the motors
  if(!left.init() || !right.init()) {
    printf("Cannot initialise the motors. Check the provided parameters\n");
    return 0;
  }

  // Demo each of the move methods
  forward();
  sleep_ms(1000);

  backward();
  sleep_ms(1000);

  curve_forward_right();
  sleep_ms(1000);

  curve_forward_left();
  sleep_ms(1000);

  turn_right();
  sleep_ms(1000);

  forward(0.5 * DRIVING_SPEED);     // Half speed
  sleep_ms(1000);

  turn_left(0.5 * DRIVING_SPEED);   // Half speed
  sleep_ms(1000);

  curve_backward_right(0.75 * DRIVING_SPEED);   // Three quarters speed
  sleep_ms(1000);

  forward();  // Full speed
  sleep_ms(500);

  coast();  // Come to a halt gently
  sleep_ms(1000);

  forward();
  sleep_ms(500);

  stop(); // Apply the brakes
  sleep_ms(1000);

  // Disable the motors
  left.disable();
  right.disable();
}
