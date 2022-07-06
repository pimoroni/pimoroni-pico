#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "enviro_indoor.hpp"

using namespace pimoroni;
using namespace enviro;

EnviroIndoor indoor;

int main() {

  indoor.init();

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  
  while(!indoor.switch_pressed()) {}

  printf("enviro indoor starting up\n");

  indoor.init_bsec();

  while(true) {
    indoor.activity_led(100.0f);
    indoor.take_reading();
    indoor.activity_led(25.0f);
  
    // TODO: the bme688 needs a temperature offset to calibrate the output
    // how do we set and store this? on the filesystem used for logging
    // the data i suspect makes the most sense.

    printf("\n");
    printf("> temperature: %0.2f celcius, %0.2f fahrenheit, %0.2f kelvin\n", 
      indoor.temperature(CELCIUS),
      indoor.temperature(FAHRENHEIT),
      indoor.temperature(KELVIN));
    printf("> humidity:    %0.2f%%\n", indoor.humidity());
    printf("> pressure:    %0.2f hPa, %0.2f psi, %0.2f inHg, %0.2f bar\n", 
      indoor.pressure(),
      indoor.pressure(PSI),
      indoor.pressure(INHG),
      indoor.pressure(BAR));

    sleep_ms(5000);
  }

  return 0;
}
