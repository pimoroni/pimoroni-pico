#include "pico/stdlib.h"

#include "motor2040.hpp"

/*
Demonstrates how to create multiple Motor objects and control them together.
*/

using namespace motor;

// How many sweeps of the motors to perform
const uint SWEEPS = 2;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to drive the motors when sweeping
constexpr float SPEED_EXTENT = 1.0f;

// Create a motor cluster, using PIO 0 and State Machine 0
const pin_pair motor_pins[] = {motor2040::MOTOR_A, motor2040::MOTOR_B,
                               motor2040::MOTOR_C, motor2040::MOTOR_D};
const uint NUM_MOTORS = count_of(motor_pins);
MotorCluster motors = MotorCluster(pio0, 0, motor_pins, NUM_MOTORS);


int main() {
  stdio_init_all();

  // Initialise the motor cluster
  motors.init();

  // Enable all motors
  motors.enable_all();
  sleep_ms(2000);

  // Drive at full positive
  motors.all_full_positive();
  sleep_ms(2000);

  // Stop all moving
  motors.stop_all();
  sleep_ms(2000);

  // Drive at full negative
  motors.all_full_negative();
  sleep_ms(2000);

  // Coast all to a gradual stop
  motors.coast_all();
  sleep_ms(2000);

  // Do a sine speed sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < 360; i++) {
      float speed = sin(((float)i * (float)M_PI) / 180.0f) * SPEED_EXTENT;
      motors.all_to_speed(speed);
      sleep_ms(20);
    }
  }

  // Do a stepped speed sweep
  for(auto j = 0u; j < SWEEPS; j++) {
    for(auto i = 0u; i < STEPS; i++) {
      motors.all_to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
    for(auto i = 0u; i < STEPS; i++) {
      motors.all_to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT);
      sleep_ms(STEPS_INTERVAL_MS);
    }
  }

  // Disable the motors
  motors.disable_all();

  // Sleep a short time so the disable takes effect
  sleep_ms(100);
}
