#include <cstdio>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "enviro_weather.hpp"

using namespace pimoroni;
using namespace enviro;

EnviroWeather weather;

int main() {
  
  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  if(!weather.init()) {
    printf("Failed to initialised Enviro Weather\n");
    return 1;
  }

  printf("Enviro Weather starting up\n");

  while(true) {
    std::string wind_direction = weather.cardinal_wind_direction();
    int wind_speed = weather.wind_speed();

    printf("Wind is blowing %s @ %d kph\n", wind_direction.c_str(), wind_speed);

    sleep_ms(100);
  }

  return 0;
}
