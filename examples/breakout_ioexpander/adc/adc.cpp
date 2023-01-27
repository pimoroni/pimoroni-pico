#include "pico/stdlib.h"
#include <stdio.h>

#include "breakout_ioexpander.hpp"

using namespace pimoroni;

static const uint8_t IOE_ADC_PIN = 10;

BreakoutIOExpander ioe(0x18);
bool toggle = false;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  if(ioe.init()) {
    printf("IOExpander found...\n");

    // ioe.set_adc_vref(5.0f);    //Uncomment this if running the IOExpander off a 5V supply
    ioe.set_mode(IOE_ADC_PIN, IOExpander::PIN_ADC);

    while(true) {
#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, toggle);
#endif
      toggle = !toggle;

      float voltage = ioe.input_as_voltage(IOE_ADC_PIN);

      printf("Voltage: %f\n", voltage);

      sleep_ms(20);
    }
  }
  else {
    printf("IOExpander not found :'(\n");

#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, true);
#endif
  }

  return 0;
}
