#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
A demonstration of driving both of Inventor 2040 W's motor outputs through a
sequence of velocities, with the help of their attached encoders and PID control.

Press "User" to exit the program.
*/

using namespace inventor;

enum Wheels {
  LEFT = MOTOR_A,
  RIGHT = MOTOR_B,
};
const char* NAMES[] = {"LEFT", "RIGHT"};

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many times to update the motor per second
const uint UPDATES = 100;
constexpr float UPDATE_RATE = 1.0f / (float)UPDATES;

// The time to travel between each random value
constexpr float TIME_FOR_EACH_MOVE = 2.0f;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// The speed to drive the wheels at
constexpr float DRIVING_SPEED = 1.0f;

// PID values
constexpr float VEL_KP = 30.0f;   // Velocity proportional (P) gain
constexpr float VEL_KI = 0.0f;    // Velocity integral (I) gain
constexpr float VEL_KD = 0.4f;    // Velocity derivative (D) gain

// LED Constants

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// The factor to convert between motor speed and LED cycle rate
constexpr float SPEED_TO_CYCLING = 0.02f / SPEED_SCALE;

// The percentage of the colour spectrum to have the LEDs gradient over
constexpr float BAR_GRADIENT = 0.125f;
const uint HALF_LEDS = NUM_LEDS / 2;


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);

// Create an array of PID pointers
PID vel_pids[NUM_MOTORS];

// LED Hue Variables
float offset_l;
float offset_r;


// Helper functions for driving in common directions
void drive_forward(float speed) {
  vel_pids[LEFT].setpoint = speed;
  vel_pids[RIGHT].setpoint = speed;
  offset_l += speed * SPEED_TO_CYCLING;
  offset_r += speed * SPEED_TO_CYCLING;
}

void turn_right(float speed) {
  vel_pids[LEFT].setpoint = speed;
  vel_pids[RIGHT].setpoint = -speed;
  offset_l += speed * SPEED_TO_CYCLING;
  offset_r -= speed * SPEED_TO_CYCLING;
}

void stop() {
  vel_pids[LEFT].setpoint = 0.0f;
  vel_pids[RIGHT].setpoint = 0.0f;
}


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    offset_l = 0.0f;
    offset_r = 0.0f;

    // Fill the arrays of motors, encoders, and pids, and initialise them
    for(auto i = 0u; i < NUM_MOTORS; i++) {
      board.motors[i].speed_scale(SPEED_SCALE);
      vel_pids[i] = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE);
    }

    // Reverse the direction of the left motor and encoder
    board.motors[LEFT].direction(REVERSED_DIR);
    board.encoders[LEFT].direction(REVERSED_DIR);

    // Enable all motors
    for(auto i = 0u; i < NUM_MOTORS; i++) {
      board.motors[i].enable();
    }

    uint update = 0;
    uint print_count = 0;
    uint sequence = 0;

    Encoder::Capture captures[NUM_MOTORS];

    // Continually move the motor until the user button is pressed
    while(!board.switch_pressed()) {

      // Capture the state of all the encoders
      for(auto i = 0u; i < NUM_MOTORS; i++) {
        captures[i] = board.encoders[i].capture();
      }

      for(auto i = 0u; i < NUM_MOTORS; i++) {
        // Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
        float accel = vel_pids[i].calculate(captures[i].revolutions_per_second());

        // Accelerate or decelerate the motor
        board.motors[i].speed(board.motors[i].speed() + (accel * UPDATE_RATE));
      }

      // Print out the current motor values and their setpoints, but only on every multiple
      if(print_count == 0) {
        for(auto i = 0u; i < NUM_MOTORS; i++) {
          printf("%s = %f, ", NAMES[i], captures[i].revolutions_per_second());
        }
        printf("\n");
      }

      // Increment the print count, and wrap it
      print_count = (print_count + 1) % PRINT_DIVIDER;

      update++;   // Move along in time

      // Have we reached the end of this movement?
      if(update >= UPDATES_PER_MOVE) {
        update = 0;  // Reset the counter

        // Move on to the next part of the sequence
        sequence += 1;

        // Loop the sequence back around
        if(sequence >= 5) {
          sequence = 0;
        }
      }

      // Set the motor speeds, based on the sequence
      switch(sequence) {
      case 0:
        drive_forward(DRIVING_SPEED);
        break;
      case 1:
        drive_forward(-DRIVING_SPEED);
        break;
      case 2:
        turn_right(DRIVING_SPEED);
        break;
      case 3:
        turn_right(-DRIVING_SPEED);
        break;
      default:
        stop();
        break;
      }

      if(offset_l < 0.0f) {
        offset_l += 1.0f;
      }

      if(offset_r < 0.0) {
        offset_r += 1.0;
      }

      // Update the LED bars
      for(auto i = 0u; i < HALF_LEDS; i++) {
        float hue = ((float)i / (float)HALF_LEDS) * BAR_GRADIENT;
        board.leds.set_hsv(i, hue + offset_l, 1.0, BRIGHTNESS);
        board.leds.set_hsv(NUM_LEDS - i - 1, hue + offset_r, 1.0, BRIGHTNESS);
      }

      sleep_ms(UPDATE_RATE * 1000.0f);
    }

    // Stop all the motors
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].disable();
    }

    // Turn off the LED bars
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}
