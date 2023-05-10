#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
Output a sine wave PWM sequence on the Encoder Wheel's side GPIO pins.

Press the centre button to stop the program.
*/

// Constants
constexpr float SPEED = 5.0f;         // The speed that the LEDs will cycle at
const uint UPDATES = 50;              // How many times the LEDs will be updated per second
const uint UPDATE_RATE_US = 1000000 / UPDATES;
constexpr float FREQUENCY = 1000.0f;  // The frequency to run the PWM at

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
float offset = 0.0f;


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {

    // Set the PWM frequency for the GPIOs
    uint16_t period = wheel.gpio_pwm_frequency(FREQUENCY);

    // Set the GPIO pins to PWM outputs
    for(int i = 0; i < NUM_GPIOS; i++) {
      wheel.gpio_pin_mode(GPIOS[i], IOExpander::PIN_PWM);
    }

    // Loop forever
    while(!wheel.pressed(CENTRE)) {
      // Record the start time of this loop
      absolute_time_t start_time = get_absolute_time();

      offset += SPEED / 1000.0f;

      // Update all the PWMs
      for(int i = 0; i < NUM_GPIOS; i++) {
        float angle = (((float)i / NUM_GPIOS) + offset) * M_PI;
        uint16_t duty = (uint16_t)(((sinf(angle) / 2.0f) + 0.5f) * period);

        // Set the GPIO pin to the new duty cycle, but do not load it yet
        wheel.gpio_pin_value(GPIOS[i], duty, false);
      }

      // Have all the PWMs load at once
      wheel.gpio_pwm_load();

      // Sleep until the next update, accounting for how long the above operations took to perform
      sleep_until(delayed_by_us(start_time, UPDATE_RATE_US));
    }

    // Turn off the PWM outputs
    for(int i = 0; i < NUM_GPIOS; i++) {
      wheel.gpio_pin_value(GPIOS[i], 0);
    }
  }

  return 0;
}