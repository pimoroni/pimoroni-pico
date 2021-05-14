#include "pico/stdlib.h"
#include <stdio.h>

#include "breakout_ioexpander.hpp"

using namespace pimoroni;

//Connect a button between this pin and ground
static const uint8_t IOE_BUTTON_PIN = 14;

BreakoutIOExpander ioe(0x18);
bool last_state = true;

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  stdio_init_all();

  if(ioe.init()) {
    printf("IOExpander found...\n");

    ioe.set_mode(IOE_BUTTON_PIN, IOExpander::PIN_IN_PULL_UP);

    while(true) {
      bool state = ioe.input(IOE_BUTTON_PIN);
      if(state != last_state) {
        if(state) {
          printf("Button has been released\n");
          gpio_put(PICO_DEFAULT_LED_PIN, false);
        }
        else {
          printf("Button has been pressed\n");
          gpio_put(PICO_DEFAULT_LED_PIN, true);
        }

        last_state = state;
      }

      sleep_ms(20);
    }
  }
  else {
    printf("IOExpander not found :'(\n");
    gpio_put(PICO_DEFAULT_LED_PIN, true);
  }

  return 0;
}
