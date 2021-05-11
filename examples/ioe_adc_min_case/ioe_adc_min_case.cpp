#include <stdio.h>
#include "pico/stdlib.h"
#include "ioexpander.hpp"

static const uint8_t PIN_TERM_A = 12;
static const uint8_t PIN_TERM_B = 3;
static const uint8_t PIN_INPUT = 11;

using namespace pimoroni;

IOExpander ioe(0x0e);

int main() {
  stdio_init_all();

  sleep_ms(5000); //Delay to give chance to connect terminal

  const uint LED_PIN = 25;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  if(ioe.init()) {
    printf("IOExpander found...\n");

    ioe.set_mode(PIN_TERM_A, IOExpander::PIN_OUT);
    ioe.set_mode(PIN_TERM_B, IOExpander::PIN_OUT);
    ioe.set_mode(PIN_INPUT, IOExpander::PIN_ADC);

    ioe.output(PIN_TERM_A, 0);
    ioe.output(PIN_TERM_B, 1);

    bool toggle = false;
    while(true) {
      gpio_put(LED_PIN, toggle);
      toggle = !toggle;

      printf("%d\n", ioe.input(PIN_INPUT));

      sleep_ms(20);
    }  
  }
  else {
    printf("IOExpander not found :'(\n");
    gpio_put(LED_PIN, 1);
  }
  
  return 0;
}
