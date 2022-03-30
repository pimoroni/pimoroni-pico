#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
Show how to read the 6 external sensors and
display their values on the neighbouring LEDs.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace servo;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// How many times to update LEDs and Servos per second
const uint UPDATES = 50;

// Set up the shared analog inputs
Analog sen_adc = Analog(servo2040::SHARED_ADC);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

// Create the LED bar, using PIO 1 and State Machine 0
WS2812 led_bar = WS2812(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);

// Create the user button
Button user_sw(servo2040::USER_SW);


int main() {
  stdio_init_all();

  // Set up the sensor addresses with pull downs
  for(auto i = 0u; i < servo2040::NUM_SENSORS; i++) {
    mux.configure_pulls(servo2040::SENSOR_1_ADDR + i, false, true);
  }

  // Start updating the LED bar
  led_bar.start();

  // Read sensors until the user button is pressed
  while(!user_sw.raw()) {

    // Read each sensor in turn and print its voltage
    for(auto i = 0u; i < servo2040::NUM_SENSORS; i++) {
      mux.select(servo2040::SENSOR_1_ADDR + i);
      float sensor_voltage = sen_adc.read_voltage();

      // Calculate the LED's hue, with Green for high voltages and Blue for low
      float hue = (2.0f - (sensor_voltage / 3.3f)) * 0.333f;
      led_bar.set_hsv(i, hue, 1.0f, BRIGHTNESS);

      printf("S%d = %f,", i + 1, sensor_voltage);
    }
    printf("\n");

    sleep_ms(1000 / UPDATES);
  }

  // Turn off the LED bar
  led_bar.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
