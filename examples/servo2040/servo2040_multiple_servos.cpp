#include "pico/stdlib.h"

#include "servo2040.hpp"

/*
Demonstrates how to create multiple Servo objects and control them together.
*/

using namespace servo;

// How many sweeps of the servo to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to move the servo when sweeping
constexpr float SWEEP_EXTENT = 90.0f;

// Create an array of servo pointers
const uint START_PIN = servo2040::SERVO_1;
const uint END_PIN = servo2040::SERVO_4;
const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;
Servo *servos[NUM_SERVOS];


int main() {
  stdio_init_all();

  // Fill the array of servos for pins 0 to 3, and initialise them. Up to 16 servos can be created
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s] = new Servo(s + START_PIN);
    servos[s]->init();
  }

  // Enable all servos (this puts them at the middle)
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s]->enable();
  }
  sleep_ms(2000);

  // Go to min
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s]->to_min();
  }
  sleep_ms(2000);

  // Go to max
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s]->to_max();
  }
  sleep_ms(2000);

  // Go back to mid
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s]->to_mid();
  }
  sleep_ms(2000);

  // Do a sine sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < 360; i++) {
      float value = sin(((float)i * (float)M_PI) / 180.0f) * SWEEP_EXTENT;
      for(auto s = 0u; s < NUM_SERVOS; s++) {
        servos[s]->set_value(value);
      }
      sleep_ms(20);
    }
  }

  // Do a stepped sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < STEPS; i++) {
      for(auto s = 0u; s < NUM_SERVOS; s++) {
        servos[s]->to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      }
      sleep_ms(STEPS_INTERVAL_MS);
    }
    for(auto i = 0u; i < STEPS; i++) {
      for(auto s = 0u; s < NUM_SERVOS; s++) {
        servos[s]->to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      }
      sleep_ms(STEPS_INTERVAL_MS);
    }
  }

  // Disable the servos
  for(auto s = 0u; s < NUM_SERVOS; s++) {
    servos[s]->disable();
    delete servos[s];
  }
}
