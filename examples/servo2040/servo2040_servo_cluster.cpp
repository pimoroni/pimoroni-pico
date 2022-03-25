#include "pico/stdlib.h"

#include "servo2040.hpp"

/*
Demonstrates how to create a ServoCluster object to control multiple servos at once.
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

// Create a servo cluster for pins 0 to 3, using PIO 0 and State Machine 0
const uint START_PIN = servo2040::SERVO_1;
const uint END_PIN = servo2040::SERVO_4;
const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;
ServoCluster servos = ServoCluster(pio0, 0, START_PIN, NUM_SERVOS);


int main() {
  stdio_init_all();

  // Initialise the servo cluster
  servos.init();

  // Enable all servos (this puts them at the middle)
  servos.enable_all();
  sleep_ms(2000);

  // Go to min
  servos.all_to_min();
  sleep_ms(2000);

  // Go to max
  servos.all_to_max();
  sleep_ms(2000);

  // Go back to mid
  servos.all_to_mid();
  sleep_ms(2000);

  // Do a sine sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < 360; i++) {
      servos.set_all_values(sin(((float)i * (float)M_PI) / 180.0f) * SWEEP_EXTENT);
      sleep_ms(20);
    }
  }

  // Do a stepped sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < STEPS; i++) {
      servos.all_to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
    for(auto i = 0u; i < STEPS; i++) {
      servos.all_to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
  }

  // Disable the servos
  servos.disable_all();
}
