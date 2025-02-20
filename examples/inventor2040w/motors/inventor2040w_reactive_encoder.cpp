#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
A demonstration of how a motor with an encoder can be used
as a programmable rotary encoder for user input, with
force-feedback for arbitrary detents and end stops.

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

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// Multipliers for the different printed values, so they appear nicely on the Thonny plotter
constexpr float SPD_PRINT_SCALE = 20.0f;    // Driving Speed multipler

// The size (in degrees) of each detent region
constexpr float DETENT_SIZE = 20.0f;

// The minimum detent that can be counted to
const int MIN_DETENT = 0;

// The maximum detent that can be counted to
const int MAX_DETENT = NUM_LEDS - 1;

// The maximum drive force (as a percent) to apply when crossing detents
constexpr float MAX_DRIVE_PERCENT = 0.5f;

// The brightness of the RGB LED
constexpr float BRIGHTNESS = 0.4f;

// PID values
constexpr float POS_KP = 0.14f;   // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.002f;  // Position derivative (D) gain


// Create a new Inventor object
Inventor board;

// Create PID object for position control
PID pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE);

int current_detent = 0;


// Function to deal with a detent change
void detent_change(int change) {
  // Update the current detent and pid setpoint
  current_detent += change;

  // Update the motor position setpoint
  pos_pid.setpoint = (current_detent * DETENT_SIZE);
  printf("Detent = %d\n", current_detent);

  // Convert the current detent to a hue and set the onboard led to it
  float hue = (float)(current_detent - MIN_DETENT) / (float)(MAX_DETENT - MIN_DETENT);
  for(auto i = 0u; i < NUM_LEDS; i++) {
    if((current_detent >= 0) && ((uint)current_detent == i))
      board.leds.set_hsv(i, hue, 1.0, BRIGHTNESS);
    else
      board.leds.set_hsv(i, hue, 1.0, 0.0);
  }
}


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

    // Call the function once to set the setpoint and print the value
    detent_change(0);

    // Continually move the motor until the user button is pressed
    while(!board.switch_pressed()) {

      // Capture the state of the encoder
      Encoder::Capture capture = enc.capture();

      // Get the current detent's centre angle
      float detent_angle = ((float)current_detent * DETENT_SIZE);

      // Is the current angle above the region of this detent?
      if(capture.degrees() > detent_angle + (DETENT_SIZE / 2)) {
        // Is there another detent we can move to?
        if(current_detent < MAX_DETENT) {
          detent_change(1);   // Increment to the next detent
        }
      }
      // Is the current angle below the region of this detent?
      else if(capture.degrees() < detent_angle - (DETENT_SIZE / 2)) {
        // Is there another detent we can move to?
        if(current_detent > MIN_DETENT) {
          detent_change(-1);  // Decrement to the next detent
        }
      }

      // Calculate the velocity to move the motor closer to the position setpoint
      float vel = pos_pid.calculate(capture.degrees(), capture.degrees_per_second());

      // If the current angle is within the detent range, limit the max vel
      // (aka feedback force) that the user will feel when turning the motor between detents
      if((capture.degrees() >= MIN_DETENT * DETENT_SIZE) && (capture.degrees() <= MAX_DETENT * DETENT_SIZE)) {
        vel = CLAMP(vel, -MAX_DRIVE_PERCENT, MAX_DRIVE_PERCENT);
      }

      // Set the new motor driving speed
      m.speed(vel);

      sleep_ms(UPDATE_RATE * 1000.0f);
    }

    // Disable the motor
    m.disable();

    // Turn off the LEDs
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}
