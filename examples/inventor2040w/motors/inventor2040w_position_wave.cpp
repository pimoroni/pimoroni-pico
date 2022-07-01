#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
A demonstration of driving both of Inventor 2040 W's motor outputs between
positions, with the help of their attached encoders and PID control.

Press "User" to exit the program.
*/

using namespace inventor;

const char* ENCODER_NAMES[] = {"A", "B"};

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

// The brightness of the RGB LED
constexpr float BRIGHTNESS = 0.4f;

// PID values
constexpr float POS_KP = 0.14f;   // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.002f;  // Position derivative (D) gain


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);

// Create an array of PID pointers
PID pos_pids[NUM_MOTORS];


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Fill the arrays of motors, encoders, and pids, and initialise them
    for(auto i = 0u; i < NUM_MOTORS; i++) {
      board.motors[i].speed_scale(SPEED_SCALE);
      pos_pids[i] = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE);
    }

    // Reverse the direction of the left motor and encoder
    board.motors[MOTOR_A].direction(REVERSED_DIR);
    board.encoders[MOTOR_A].direction(REVERSED_DIR);

    // Enable all motors
    for(auto i = 0u; i < NUM_MOTORS; i++) {
      board.motors[i].enable();
    }

    uint update = 0;
    uint print_count = 0;

    // Set the initial and end values
    float start_value = 0.0f;
    float end_value = 270.0f;

    Encoder::Capture captures[NUM_MOTORS];

    // Continually move the motor until the user button is pressed
    while(!board.switch_pressed()) {

      // Capture the state of all the encoders
      for(auto i = 0u; i < NUM_MOTORS; i++) {
        captures[i] = board.encoders[i].capture();
      }

      // Calculate how far along this movement to be
      float percent_along = (float)update / (float)UPDATES_PER_MOVE;

      for(auto i = 0u; i < NUM_MOTORS; i++) {
        // Move the motor between values using cosine
        pos_pids[i].setpoint = (((-cosf(percent_along * (float)M_PI) + 1.0) / 2.0) * (end_value - start_value)) + start_value;

        // Calculate the velocity to move the motor closer to the position setpoint
        float vel = pos_pids[i].calculate(captures[i].degrees(), captures[i].degrees_per_second());

        // Set the new motor driving speed
        board.motors[i].speed(vel);
      }

      // Update the LEDs
      board.leds.set_hsv(LED_GP0, percent_along, 1.0, BRIGHTNESS);
      board.leds.set_hsv(LED_SERVO_6, percent_along, 1.0, BRIGHTNESS);

      // Print out the current motor values and their setpoints, but only on every multiple
      if(print_count == 0) {
        for(auto i = 0u; i < NUM_MOTORS; i++) {
          printf("%s = %f, ", ENCODER_NAMES[i], captures[i].degrees());
        }
        printf("\n");
      }

      // Increment the print count, and wrap it
      print_count = (print_count + 1) % PRINT_DIVIDER;

      update++;   // Move along in time

      // Have we reached the end of this movement?
      if(update >= UPDATES_PER_MOVE) {
        update = 0;  // Reset the counter

        // Swap the start and end values
        float temp = start_value;
        start_value = end_value;
        end_value = temp;
      }

      sleep_ms(UPDATE_RATE * 1000.0f);
    }

    // Stop all the motors
    for(auto m = 0u; m < NUM_MOTORS; m++) {
      board.motors[m].disable();
    }

    // Turn off the LED
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}
