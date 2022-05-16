#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
Shows how to initialise and read the 2 external
and 6 internal sensors of Motor 2040.

Press "Boot" to exit the program.
*/

using namespace motor;

// Set up the shared analog inputs
Analog sen_adc = Analog(motor2040::SHARED_ADC);
Analog vol_adc = Analog(motor2040::SHARED_ADC, motor2040::VOLTAGE_GAIN);
Analog cur_adc = Analog(motor2040::SHARED_ADC, motor2040::CURRENT_GAIN,
                        motor2040::SHUNT_RESISTOR, motor2040::CURRENT_OFFSET);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(motor2040::ADC_ADDR_0, motor2040::ADC_ADDR_1, motor2040::ADC_ADDR_2,
                          PIN_UNUSED, motor2040::SHARED_ADC);

// Create the user button
Button user_sw(motor2040::USER_SW);


int main() {
  stdio_init_all();

  // Set up the sensor addresses with pull downs
  for(auto i = 0u; i < motor2040::NUM_SENSORS; i++) {
    mux.configure_pulls(motor2040::SENSOR_1_ADDR + i, false, true);
  }

  // Set up the pull-up for the fault sense
  mux.configure_pulls(motor2040::FAULT_SENSE_ADDR, true, false);

  // Read sensors until the user button is pressed
  while(!user_sw.raw()) {

    // Read each sensor in turn and print its voltage
    for(auto i = 0u; i < motor2040::NUM_SENSORS; i++) {
      mux.select(motor2040::SENSOR_1_ADDR + i);
      printf("S%d = %f, ", i + 1, sen_adc.read_voltage());
    }

    // Read the voltage sense and print the value
    mux.select(motor2040::VOLTAGE_SENSE_ADDR);
    printf("Voltage = %f, ", vol_adc.read_voltage());

    // Read the current sense's of each motor and print the value
    for(auto i = 0u; i < motor2040::NUM_MOTORS; i++) {
      mux.select(motor2040::CURRENT_SENSE_A_ADDR + i);
      printf("C%d = %f, ", i + 1, cur_adc.read_current());
    }

    // Read the fault sense and print the value
    mux.select(motor2040::FAULT_SENSE_ADDR);
    printf("Fault = %s\n", mux.read() ? "false" : "true");

    sleep_ms(500);
  }
}
