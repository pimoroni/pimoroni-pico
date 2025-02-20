#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to control a single servo on Inventor 2040/2350 W.
*/

using namespace inventor;

// How many sweeps of the servo to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to move the servo when sweeping
constexpr float SWEEP_EXTENT = 90.0f;


// Create a new Inventor object
Inventor board;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the servo from Inventor
    Servo& s = board.servos[SERVO_1];

    // Enable the servo (this puts it at the middle)
    s.enable();
    sleep_ms(2000);

    // Go to min
    s.to_min();
    sleep_ms(2000);

    // Go to max
    s.to_max();
    sleep_ms(2000);

    // Go back to mid
    s.to_mid();
    sleep_ms(2000);

    // Do a sine sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < 360; i++) {
        s.value(sin(((float)i * (float)M_PI) / 180.0f) * SWEEP_EXTENT);
        sleep_ms(20);
      }
    }

    // Do a stepped sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < STEPS; i++) {
        s.to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
        sleep_ms(STEPS_INTERVAL_MS);
      }
      for(auto i = 0u; i < STEPS; i++) {
        s.to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
        sleep_ms(STEPS_INTERVAL_MS);
      }
    }

    // Disable the servo
    s.disable();
  }
}
