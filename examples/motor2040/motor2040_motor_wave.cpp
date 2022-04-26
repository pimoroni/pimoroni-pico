#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"

/*
An example of applying a wave pattern to a group of motors and the LED.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace motor;

// The speed that the LEDs and motors will cycle at
const uint SPEED = 5;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4;

// How many times to update LEDs and motors per second
const uint UPDATES = 50;

// How far from zero to drive the motors
constexpr float SPEED_EXTENT = 1.0f;


// Create an array of motor pointers
const pin_pair motor_pins[] = {motor2040::MOTOR_A, motor2040::MOTOR_B,
                               motor2040::MOTOR_C, motor2040::MOTOR_D};
const uint NUM_MOTORS = count_of(motor_pins);
Motor *motors[NUM_MOTORS];

// Create the LED, using PIO 1 and State Machine 0
WS2812 led(motor2040::NUM_LEDS, pio1, 0, motor2040::LED_DATA);

// Create the user button
Button user_sw(motor2040::USER_SW);


int main() {
  stdio_init_all();

  // Fill the array of motors, and initialise them. Up to 8 motors can be created
  for(auto m = 0u; m < NUM_MOTORS; m++) {
    motors[m] = new Motor(motor_pins[m]);
    motors[m]->init();
  }

  // Start updating the LED
  led.start();

  float offset = 0.0f;

  // Make rainbows until the user button is pressed
  while(!user_sw.raw()) {

    offset += (float)SPEED / 1000.0f;

    // Update the LED
    led.set_hsv(0, offset / 2.0f, 1.0f, BRIGHTNESS);

    // Update all the motors
      for(auto m = 0u; m < NUM_MOTORS; m++) {
        float angle = (((float)m / (float)NUM_MOTORS) + offset) * (float)M_TWOPI;
        motors[m]->speed(sin(angle) * SPEED_EXTENT);
    }

    sleep_ms(1000 / UPDATES);
  }

  // Stop all the motors
  for(auto m = 0u; m < NUM_MOTORS; m++) {
    motors[m]->disable();
  }

  // Turn off the LED
  led.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
