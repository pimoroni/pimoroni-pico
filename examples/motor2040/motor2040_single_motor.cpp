#include "pico/stdlib.h"

#include "motor2040.hpp"

/*
Demonstrates how to create a Motor object and control it.
*/

using namespace motor;

// How many sweeps of the motor to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to move the motor when sweeping
constexpr float SWEEP_EXTENT = 90.0f;


// Create a motor on pin 0 and 1
Motor2 m = Motor2(motor2040::MOTOR_1);


int main() {
  stdio_init_all();

  // Initialise the motor
  m.init();

  // Enable the motor
  m.enable();
  sleep_ms(2000);

  // Go at full neative
  m.to_full_negative();
  sleep_ms(2000);

  // Go at full positive
  m.to_full_positive();
  sleep_ms(2000);

  // Stop the motor
  m.stop();
  sleep_ms(2000);

  // Do a sine sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < 360; i++) {
      m.speed(sin(((float)i * (float)M_PI) / 180.0f) * SWEEP_EXTENT);
      sleep_ms(20);
    }
  }

  // Do a stepped sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < STEPS; i++) {
      m.to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
    for(auto i = 0u; i < STEPS; i++) {
      m.to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
  }

  // Disable the motor
  m.disable();
}
