#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"
#include "pid.hpp"

/*
A demonstration of how a motor with an encoder can be used
as a programmable rotary encoder for user input, with
force-feedback for arbitrary detents and end stops.

Press "Boot" to exit the program.
*/

using namespace plasma;
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

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// Multipliers for the different printed values, so they appear nicely on the Thonny plotter
constexpr float SPD_PRINT_SCALE = 20.0f;    // Driving Speed multipler

// The size (in degrees) of each detent region
constexpr float DETENT_SIZE = 20.0f;

// The minimum detent that can be counted to
const int MIN_DETENT = -12;

// The maximum detent that can be counted to
const int MAX_DETENT = +12;

// The maximum drive force (as a percent) to apply when crossing detents
constexpr float MAX_DRIVE_PERCENT = 0.5f;

// The brightness of the RGB LED
constexpr float BRIGHTNESS = 0.4f;

// PID values
constexpr float POS_KP = 0.14f;   // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.002f;  // Position derivative (D) gain


// Create a motor and set its direction and speed scale
Motor m = Motor(MOTOR_PINS, DIRECTION, SPEED_SCALE);

// Create an encoder and set its direction and counts per rev, using PIO 0 and State Machine 0
Encoder enc = Encoder(pio0, 0, ENCODER_PINS, PIN_UNUSED, DIRECTION, COUNTS_PER_REV, true);

// Create the user button
Button user_sw(motor2040::USER_SW);

// Create the LED, using PIO 1 and State Machine 0
WS2812 led(motor2040::NUM_LEDS, pio1, 0, motor2040::LED_DATA);

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
  led.set_hsv(0, hue, 1.0, BRIGHTNESS);
}


int main() {
  stdio_init_all();

  // Initialise the motor and encoder
  m.init();
  enc.init();

  // Start updating the LED
  led.start();

  // Enable the motor
  m.enable();

  // Call the function once to set the setpoint and print the value
  detent_change(0);

  // Continually move the motor until the user button is pressed
  while(!user_sw.raw()) {

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

  // Turn off the LED
  led.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
