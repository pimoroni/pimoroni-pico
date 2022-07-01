#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to control both motors on Inventor 2040 W.
*/

using namespace inventor;

// How many sweeps of the motors to perform
const uint SWEEPS = 2;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to drive the motors when sweeping
constexpr float SPEED_EXTENT = 1.0f;


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

    // Attempt to initialise the board
  if(board.init()) {

    // Enable all motors
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].enable();
    }
    sleep_ms(2000);

    // Drive at full positive
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].full_positive();
    }
    sleep_ms(2000);

    // Stop all moving
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].stop();
    }
    sleep_ms(2000);

    // Drive at full negative
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].full_negative();
    }
    sleep_ms(2000);

    // Coast all to a gradual stop
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].coast();
    }
    sleep_ms(2000);

    // Do a sine speed sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < 360; i++) {
        float speed = sin(((float)i * (float)M_PI) / 180.0f) * SPEED_EXTENT;
        for(auto m = 0u; m < NUM_MOTORS; m++) {
          board.motors[m].speed(speed);
        }
        sleep_ms(20);
      }
    }

    // Do a stepped speed sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < STEPS; i++) {
        for(auto m = 0u; m < NUM_MOTORS; m++) {
          board.motors[m].to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
        }
        sleep_ms(STEPS_INTERVAL_MS);
      }
      for(auto i = 0u; i < STEPS; i++) {
        for(auto m = 0u; m < NUM_MOTORS; m++) {
          board.motors[m].to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
        }
        sleep_ms(STEPS_INTERVAL_MS);
      }
    }

    // Disable the motors
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].disable();
    }
  }
}
