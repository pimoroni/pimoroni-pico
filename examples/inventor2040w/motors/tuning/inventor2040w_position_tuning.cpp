#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
A program to aid in the discovery and tuning of motor PID
values for position control. It does this by commanding the
motor to move repeatedly between two setpoint angles and
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
constexpr float SPD_PRINT_SCALE = 10.0f;    // Driving Speed multipler

// How far from zero to move the motor, in degrees
constexpr float POSITION_EXTENT = 90.0f;

// PID values
constexpr float POS_KP = 0.14f;   // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.002f;  // Position derivative (D) gain


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);

// Create PID object for position control
PID pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE);


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the motor and encoder from Inventor
    Motor& m = board.motors[MOTOR_A];
    Encoder& enc = board.encoders[MOTOR_A];

    //# Set the motor's speed scale
    m.speed_scale(SPEED_SCALE);

    // Set the motor and encoder's direction
    m.direction(DIRECTION);
    enc.direction(DIRECTION);

    // Enable the motor
    m.enable();

    // Set the initial setpoint position
    pos_pid.setpoint = POSITION_EXTENT;


    uint update = 0;
    uint print_count = 0;

    // Continually move the motor until the user button is pressed
    while(!board.switch_pressed()) {

      // Capture the state of the encoder
      Encoder::Capture capture = enc.capture();

      // Calculate the velocity to move the motor closer to the position setpoint
      float vel = pos_pid.calculate(capture.degrees(), capture.degrees_per_second());

      // Set the new motor driving speed
      m.speed(vel);

      // Print out the current motor values and their setpoints,
      // but only for the first few updates and only every multiple
      if(update < (uint)(PRINT_WINDOW * UPDATES) && print_count == 0) {
        printf("Pos = %f, ", capture.degrees());
        printf("Pos SP = %f, ", pos_pid.setpoint);
        printf("Speed = %f\n", m.speed() * SPD_PRINT_SCALE);
      }

      // Increment the print count, and wrap it
      print_count = (print_count + 1) % PRINT_DIVIDER;

      update++;   // Move along in time

      // Have we reached the end of this time window?
      if(update >= (uint)(MOVEMENT_WINDOW * UPDATES)) {
        update = 0;  // Reset the counter

        // Set the new position setpoint to be the inverse of the current setpoint
        pos_pid.setpoint = 0.0f - pos_pid.setpoint;
      }

      sleep_ms(UPDATE_RATE * 1000.0f);
    }

    // Disable the motor
    m.disable();
  }
}
