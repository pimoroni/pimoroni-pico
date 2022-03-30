#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
An example of how to use Servo 2040's current measuring
ability and display the value on the onboard LED bar.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace servo;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// How many times to update LEDs and Servos per second
const uint UPDATES = 50;

// The maximum current, in amps, to show on the meter
constexpr float MAX_CURRENT = 3.0f;

// The number of current measurements to take per reading
const uint SAMPLES = 50;

// The time between each current measurement
const uint TIME_BETWEEN_MS = 1;

// Create a servo cluster for pins 0 to 7, using PIO 0 and State Machine 0
const uint START_PIN = servo2040::SERVO_1;
const uint END_PIN = servo2040::SERVO_8;
const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;
ServoCluster servos = ServoCluster(pio0, 0, START_PIN, NUM_SERVOS);

// Set up the shared analog inputs
Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

// Create the LED bar, using PIO 1 and State Machine 0
WS2812 led_bar(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);

// Create the user button
Button user_sw(servo2040::USER_SW);


int main() {
  stdio_init_all();

  // Initialise the servo cluster
  servos.init();

  // Start updating the LED bar
  led_bar.start();

  // Enable all servos (this puts them at the middle).
  // The servos are not going to be moved, but are activated to give a current draw
  servos.enable_all();

  // Read sensors until the user button is pressed
  while(!user_sw.raw()) {

    // Select the current sense
    mux.select(servo2040::CURRENT_SENSE_ADDR);

    // Read the current sense several times and average the result
    float current = 0.0f;
    for(auto i = 0u; i < SAMPLES; i++) {
        current += cur_adc.read_current();
        sleep_ms(TIME_BETWEEN_MS);
    }
    current /= SAMPLES;

    // Print out the current sense value
    printf("Current = %f\n", current);

    // Convert the current to a percentage of the maximum we want to show
    float percent = (current / MAX_CURRENT);

    // Update all the LEDs
    for(auto i = 0u; i < servo2040::NUM_LEDS; i++) {
      // Calculate the LED's hue, with Red for high currents and Green for low
      float hue = (1.0f - (float)i / (float)(servo2040::NUM_LEDS - 1)) * 0.333f;

      // Calculate the current level the LED represents
      float level = (i + 0.5f) / servo2040::NUM_LEDS;
      // If the percent is above the level, light the LED, otherwise turn it off
      if(percent >= level)
        led_bar.set_hsv(i, hue, 1.0f, BRIGHTNESS);
      else
        led_bar.set_hsv(i, hue, 1.0f, 0.0f);
    }
  }

  // Stop all the servos
  servos.disable_all();

  // Turn off the LED bar
  led_bar.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
