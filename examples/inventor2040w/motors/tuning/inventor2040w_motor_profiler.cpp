#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"

/*
A program that profiles the speed of a motor across its PWM
duty cycle range using the attached encoder for feedback.
*/

using namespace inventor;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50;

// The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
const Direction DIRECTION = NORMAL_DIR;

// The scaling to apply to the motor's speed. Set this to the maximum measured speed
constexpr float SPEED_SCALE = 5.4f;

// The duty cycle that corresponds with zero speed when plotting the motor's speed as a straight line
constexpr float ZERO_POINT = 0.0f;

// The duty cycle below which the motor's friction prevents it from moving
constexpr float DEAD_ZONE = 0.0f;

// How many duty cycle steps to sample the speed of
const uint DUTY_STEPS = 100;

// How long to wait after changing motor duty cycle
const uint SETTLE_TIME = 100;

// How long to capture the motor's speed at each step
const uint CAPTURE_TIME = 200;


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);


// Function that performs a single profiling step
void profile_at_duty(Motor& m, Encoder& enc, float duty) {
  // Set the motor to a new duty cycle and wait for it to settle
  if(DIRECTION == REVERSED_DIR)
      m.duty(0.0 - duty);
  else
      m.duty(duty);
  sleep_ms(SETTLE_TIME);

  // Perform a dummy capture to clear the encoder
  enc.capture();

  // Wait for the capture time to pass
  sleep_ms(CAPTURE_TIME);

  // Perform a capture and read the measured speed
  Encoder::Capture capture = enc.capture();
  float measured_speed = capture.revolutions_per_second();

  // These are some alternate speed measurements from the encoder
  // float measured_speed = capture.revolutions_per_minute();
  // float measured_speed = capture.degrees_per_second();
  // float measured_speed = capture.radians_per_second();

  // Print out the expected and measured speeds, as well as their difference
  printf("Duty = %f, Expected = %f, Measured = %f, Diff = %f\n",
         m.duty(), m.speed(), measured_speed, m.speed() - measured_speed);
}


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Give some time to connect up a serial terminal
    sleep_ms(10000);

    // Access the motor and encoder from Inventor
    Motor& m = board.motors[MOTOR_A];
    Encoder& enc = board.encoders[MOTOR_A];

    // Set the motor's speed scale, zeropoint, and deadzone
    m.speed_scale(SPEED_SCALE);
    m.zeropoint(ZERO_POINT);
    m.deadzone(DEAD_ZONE);

    // Set the motor and encoder's direction
    m.direction(DIRECTION);
    enc.direction(DIRECTION);


    // Enable the motor to get started
    m.enable();

    printf("Profiler Starting...\n");

    // Profile from 0% up to one step below 100%
    for(uint i = 0; i < DUTY_STEPS; i++) {
      profile_at_duty(m, enc, (float)i / (float)DUTY_STEPS);
    }

    // Profile from 100% down to one step above 0%
    for(uint i = 0; i < DUTY_STEPS; i++) {
      profile_at_duty(m, enc, (float)(DUTY_STEPS - i) / (float)DUTY_STEPS);
    }

    // Profile from 0% down to one step above -100%
    for(uint i = 0; i < DUTY_STEPS; i++) {
      profile_at_duty(m, enc, -(float)i / (float)DUTY_STEPS);
    }

    // Profile from -100% up to one step below 0%
    for(uint i = 0; i < DUTY_STEPS; i++) {
      profile_at_duty(m, enc, -(float)(DUTY_STEPS - i) / (float)DUTY_STEPS);
    }

    // Profile 0% again
    profile_at_duty(m, enc, 0.0f);

    printf("Profiler Finished...\n");

    // Disable the motor now the profiler has finished
    m.disable();
  }
}
