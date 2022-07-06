#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "enviro_grow.hpp"

/*
An example for Enviro Grow
*/

using namespace pimoroni;
using namespace enviro;

// How many seconds to sleep between each reading
const int SECONDS_TO_SLEEP = 10;

// The friendly names to give sensor input
const char* SENSOR_NAMES[] = {"PLANT_A", "PLANT_B", "PLANT_C"};


// Create a new EnviroGrow
EnviroGrow grow;


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(grow.init()) {

    // Turn off the warning LED and enable the activity LED
    grow.warn_led(EnviroBase::WARN_LED_OFF);
    grow.activity_led(true);

    // Take readings until the user switch is pressed
    while(!grow.switch_pressed()) {
      BME280::bme280_reading result = grow.bme280.read_forced();
      printf("%s %0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", result.status == true ? "OK" : "ER", result.temperature, result.pressure, result.humidity);

      // Read each sensor in turn and print its value
      for(uint i = 0; i < NUM_SENSORS; i++) {
        uint sensor_val = grow.sensor_ticks(i, 1000);
        printf("%s = %d, ", SENSOR_NAMES[i], sensor_val);

        grow.pump(i, sensor_val > 20);
      }

      // Print a new line
      printf("\n");

      sleep_ms(100);
    }
    sleep_ms(2000);

    // Put the board to sleep for 10 seconds
    grow.sleep(10);
  }
}
