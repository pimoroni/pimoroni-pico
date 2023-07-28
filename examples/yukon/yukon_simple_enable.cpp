#include "pico/stdlib.h"

#include "yukon.hpp"
using namespace pimoroni;
/*
Demonstrates how to create a Servo object and control it.
*/

// How many sweeps of the servo to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to move the servo when sweeping
constexpr float SWEEP_EXTENT = 90.0f;


Yukon y = Yukon();


int main() {
  stdio_init_all();

  // Initialise the servo
  y.init();

  y.set_slow_output(Yukon::MAIN_EN, true);
}
