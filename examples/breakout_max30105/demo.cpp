#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "breakout_max30105.hpp"

using namespace pimoroni;

BreakoutMAX30105 max30105(i2c0, 4, 5, 3);

int main() {
  setup_default_uart();
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  if(!max30105.init()) {
    printf("MAX30105: Failed to initialize. %02x\n", max30105.get_chip_id());
  }

  while(true) {
    uint32_t samples[32 * 3];
    int num_samples = max30105.get_samples(samples);
    float temperature = max30105.get_temperature();

    printf("MAX30105: Samples: %i %f\n", num_samples);
    printf("MAX30105: Temperature: %fC\n", temperature);
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(500);
  }

  return 0;
}
