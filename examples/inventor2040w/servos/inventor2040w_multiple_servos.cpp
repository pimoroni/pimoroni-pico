#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to control all of the servos on Inventor 2040 W.
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


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Enable all servos (this puts them at the middle)
    for(auto s = 0u; s < NUM_SERVOS; s++) {
      board.servos[s].enable();
    }
    sleep_ms(2000);

    // Go to min
    for(auto s = 0u; s < NUM_SERVOS; s++) {
      board.servos[s].to_min();
    }
    sleep_ms(2000);

    // Go to max
    for(auto s = 0u; s < NUM_SERVOS; s++) {
      board.servos[s].to_max();
    }
    sleep_ms(2000);

    // Go back to mid
    for(auto s = 0u; s < NUM_SERVOS; s++) {
      board.servos[s].to_mid();
    }
    sleep_ms(2000);

    // Do a sine sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < 360; i++) {
        float value = sin(((float)i * (float)M_PI) / 180.0f) * SWEEP_EXTENT;
        for(auto s = 0u; s < NUM_SERVOS; s++) {
          board.servos[s].value(value);
        }
        sleep_ms(20);
      }
    }

    // Do a stepped sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < STEPS; i++) {
        for(auto s = 0u; s < NUM_SERVOS; s++) {
          board.servos[s].to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
        }
        sleep_ms(STEPS_INTERVAL_MS);
      }
      for(auto i = 0u; i < STEPS; i++) {
        for(auto s = 0u; s < NUM_SERVOS; s++) {
          board.servos[s].to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
        }
        sleep_ms(STEPS_INTERVAL_MS);
      }
    }

    // Disable the servos
    for(auto s = 0u; s < NUM_SERVOS; s++) {
      board.servos[s].disable();
    }
  }
}
