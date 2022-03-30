#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
Shows how to initialise and read the 6 external
and 2 internal sensors of Servo 2040.

Press "Boot" to exit the program.
*/

using namespace servo;

// Set up the shared analog inputs
Analog sen_adc = Analog(servo2040::SHARED_ADC);
Analog vol_adc = Analog(servo2040::SHARED_ADC, servo2040::VOLTAGE_GAIN);
Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

// Create the user button
Button user_sw(servo2040::USER_SW);


int main() {
  stdio_init_all();

  // Set up the sensor addresses with pull downs
  for(auto i = 0u; i < servo2040::NUM_SENSORS; i++) {
    mux.configure_pulls(servo2040::SENSOR_1_ADDR + i, false, true);
  }

  // Read sensors until the user button is pressed
  while(!user_sw.raw()) {

    // Read each sensor in turn and print its voltage
    for(auto i = 0u; i < servo2040::NUM_SENSORS; i++) {
      mux.select(servo2040::SENSOR_1_ADDR + i);
      printf("S%d = %f, ", i + 1, sen_adc.read_voltage());
    }

    // Read the voltage sense and print the value
    mux.select(servo2040::VOLTAGE_SENSE_ADDR);
    printf("Voltage = %f, ", vol_adc.read_voltage());

    // Read the current sense and print the value
    mux.select(servo2040::CURRENT_SENSE_ADDR);
    printf("Current = %f\n", cur_adc.read_current());

    sleep_ms(500);
  }
}
