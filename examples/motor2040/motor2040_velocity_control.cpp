#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"
#include "pid.hpp"

/*
An example of how to drive a motor smoothly between random speeds,
with the help of it's attached encoder and PID control.

Press "Boot" to exit the program.
*/

using namespace motor;
using namespace encoder;

// The pins of the motor being profiled
const pin_pair MOTOR_PINS = motor2040::MOTOR_A;

// The pins of the encoder attached to the profiled motor
const pin_pair ENCODER_PINS = motor2040::ENCODER_A;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The counts per revolution of the motor's output shaft
constexpr float COUNTS_PER_REV = MMME_CPR * GEAR_RATIO;

// The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
const Direction DIRECTION = NORMAL_DIR;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many times to update the motor per second
const uint UPDATES = 100;
constexpr float UPDATE_RATE = 1.0f / (float)UPDATES;

// The time to travel between each random value, in seconds
constexpr float TIME_FOR_EACH_MOVE = 1.0f;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// Multipliers for the different printed values, so they appear nicely on the Thonny plotter
constexpr float ACC_PRINT_SCALE = 0.05f;    // Acceleration multiplier

// How far from zero to drive the motor at, in revolutions per second
constexpr float VELOCITY_EXTENT = 3.0f;

// The interpolating mode between setpoints. STEP (0), LINEAR (1), COSINE (2)
const uint INTERP_MODE = 2;

// PID values
constexpr float VEL_KP = 30.0f;   // Velocity proportional (P) gain
constexpr float VEL_KI = 0.0f;    // Velocity integral (I) gain
constexpr float VEL_KD = 0.4f;    // Velocity derivative (D) gain


// Create a motor and set its direction and speed scale
Motor m = Motor(MOTOR_PINS, DIRECTION, SPEED_SCALE);

// Create an encoder and set its direction and counts per rev, using PIO 0 and State Machine 0
Encoder enc = Encoder(pio0, 0, ENCODER_PINS, PIN_UNUSED, DIRECTION, COUNTS_PER_REV, true);

// Create the user button
Button user_sw(motor2040::USER_SW);

// Create PID object for velocity control
PID vel_pid = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE);


int main() {
  stdio_init_all();

  // Initialise the motor and encoder
  m.init();
  enc.init();

  // Enable the motor
  m.enable();


  uint update = 0;
  uint print_count = 0;

  // Set the initial value and create a random end value between the extents
  float start_value = 0.0f;
  float end_value = (((float)rand() / (float)RAND_MAX) * (VELOCITY_EXTENT * 2.0f)) - VELOCITY_EXTENT;

  // Continually move the motor until the user button is pressed
  while(!user_sw.raw()) {

    // Capture the state of the encoder
    Encoder::Capture capture = enc.capture();

    // Calculate how far along this movement to be
    float percent_along = (float)update / (float)UPDATES_PER_MOVE;

    switch(INTERP_MODE) {
      case 0:
        // Move the motor instantly to the end value
        vel_pid.setpoint = end_value;
        break;

      case 2:
        // Move the motor between values using cosine
        vel_pid.setpoint = (((-cosf(percent_along * (float)M_PI) + 1.0) / 2.0) * (end_value - start_value)) + start_value;
        break;

      case 1:
      default:
        // Move the motor linearly between values
        vel_pid.setpoint = (percent_along * (end_value - start_value)) + start_value;
    }

    // Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
    float accel = vel_pid.calculate(capture.revolutions_per_second());

    // Accelerate or decelerate the motor
    m.speed(m.speed() + (accel * UPDATE_RATE));

    // Print out the current motor values and their setpoints, but only on every multiple
    if(print_count == 0) {
        printf("Vel = %f, ", capture.revolutions_per_second());
        printf("Vel SP = %f, ", vel_pid.setpoint);
        printf("Accel = %f, ", accel * ACC_PRINT_SCALE);
        printf("Speed = %f\n", m.speed());
    }

    // Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER;

    update++;   // Move along in time

    // Have we reached the end of this movement?
    if(update >= UPDATES_PER_MOVE) {
        update = 0;  // Reset the counter

        // Set the start as the last end and create a new random end value
        start_value = end_value;
        end_value = (((float)rand() / (float)RAND_MAX) * (VELOCITY_EXTENT * 2.0f)) - VELOCITY_EXTENT;
    }

    sleep_ms(UPDATE_RATE * 1000.0f);
  }

  // Disable the motor
  m.disable();
}
