#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to control a motor on Inventor 2040 W.
*/

using namespace inventor;

// How many sweeps of the motor to perform
const uint SWEEPS = 2;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to drive the motor when sweeping
constexpr float SPEED_EXTENT = 1.0f;


// Create a new Inventor2040W
Inventor2040W board;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the motor from Inventor
    Motor& m = board.motors[MOTOR_A];

    // Enable the motor
    m.enable();
    sleep_ms(2000);

    // Drive at full positive
    m.full_positive();
    sleep_ms(2000);

    // Stop moving
    m.stop();
    sleep_ms(2000);

    // Drive at full negative
    m.full_negative();
    sleep_ms(2000);

    // Coast to a gradual stop
    m.coast();
    sleep_ms(2000);


    // Do a sine speed sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < 360; i++) {
        m.speed(sin(((float)i * (float)M_PI) / 180.0f) * SPEED_EXTENT);
        sleep_ms(20);
      }
    }

    // Do a stepped speed sweep
    for(auto j = 0u; j < SWEEPS; j++) {
      for(auto i = 0u; i < STEPS; i++) {
        m.to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
        sleep_ms(STEPS_INTERVAL_MS);
      }
      for(auto i = 0u; i < STEPS; i++) {
        m.to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
        sleep_ms(STEPS_INTERVAL_MS);
      }
    }

    // Disable the motor
    m.disable();
  }
}
