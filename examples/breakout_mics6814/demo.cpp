#include "pico/stdlib.h"
#include "stdio.h"

#include "breakout_mics6814.hpp"

using namespace pimoroni;

BreakoutMICS6814 mics6814;

int main() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  mics6814.init();
  mics6814.set_heater(true);
  mics6814.set_led(0, 0, 255);

  while(true) {
    BreakoutMICS6814::Reading reading = mics6814.read_all();
    printf("OX: %f Red: %f NH3: %f\n", reading.oxidising, reading.reducing, reading.nh3);
    sleep_ms(1000);
  }

  return 0;
}
