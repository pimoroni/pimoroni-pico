#include "pico/stdlib.h"

#include "breakout_mics6814.hpp"

using namespace pimoroni;

BreakoutMICS6814 ioe;

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  ioe.init();

  while(true) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(1000);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(1000);
  }

  return 0;
}
