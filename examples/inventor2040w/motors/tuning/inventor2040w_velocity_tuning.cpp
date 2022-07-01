#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
A program to aid in the discovery and tuning of motor PID
values for velocity control. It does this by commanding the
motor to drive repeatedly between two setpoint speeds and
plots the measured response.

Press "User" to exit the program.
*/

using namespace inventor;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
const Direction DIRECTION = NORMAL_DIR;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many times to update the motor per second
const uint UPDATES = 100;
constexpr float UPDATE_RATE = 1.0f / (float)UPDATES;

// The time (in seconds) after a new setpoint, to display print out motor values
constexpr float PRINT_WINDOW = 0.25f;

// The time (in seconds) between each new setpoint being set
constexpr float MOVEMENT_WINDOW = 2.0f;

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 1;

// Multipliers for the different printed values, so they appear nicely on the Thonny plotter
constexpr float ACC_PRINT_SCALE = 0.01f;    // Acceleration multiplier

// How far from zero to drive the motor at, in revolutions per second
constexpr float VELOCITY_EXTENT = 3.0f;

// PID values
constexpr float VEL_KP = 30.0f;   // Velocity proportional (P) gain
constexpr float VEL_KI = 0.0f;    // Velocity integral (I) gain
constexpr float VEL_KD = 0.4f;    // Velocity derivative (D) gain


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);

// Create PID object for velocity control
PID vel_pid = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE);


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the motor and encoder from Inventor
    Motor& m = board.motors[MOTOR_A];
    Encoder& enc = board.encoders[MOTOR_A];

    // Set the motor's speed scale
    m.speed_scale(SPEED_SCALE);

    // Set the motor and encoder's direction
    m.direction(DIRECTION);
    enc.direction(DIRECTION);

    // Enable the motor
    m.enable();

    // Set the initial setpoint velocity
    vel_pid.setpoint = VELOCITY_EXTENT;


    uint update = 0;
    uint print_count = 0;

    // Continually move the motor until the user button is pressed
    while(!board.switch_pressed()) {

      // Capture the state of the encoder
      Encoder::Capture capture = enc.capture();

      // Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
      float accel = vel_pid.calculate(capture.revolutions_per_second());

      // Accelerate or decelerate the motor
      m.speed(m.speed() + (accel * UPDATE_RATE));

      // Print out the current motor values and their setpoints,
      // but only for the first few updates and only every multiple
      if(update < (uint)(PRINT_WINDOW * UPDATES) && print_count == 0) {
          printf("Vel = %f, ", capture.revolutions_per_second());
          printf("Vel SP = %f, ", vel_pid.setpoint);
          printf("Accel = %f, ", accel * ACC_PRINT_SCALE);
          printf("Speed = %f\n", m.speed());
      }

      // Increment the print count, and wrap it
      print_count = (print_count + 1) % PRINT_DIVIDER;

      update++;   // Move along in time

      // Have we reached the end of this time window?
      if(update >= (uint)(MOVEMENT_WINDOW * UPDATES)) {
          update = 0;  // Reset the counter

          // Set the new velocity setpoint to be the inverse of the current setpoint
          vel_pid.setpoint = 0.0f - vel_pid.setpoint;
      }

      sleep_ms(UPDATE_RATE * 1000.0f);
    }

    // Disable the motor
    m.disable();
  }
}
