#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "breakout_max30105.hpp"
#include "heartrate.hpp"

using namespace pimoroni;

BreakoutMAX30105 max30105(i2c0, 4, 5, 3);

int main() {
  setup_default_uart();
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  // Init with 2 LEDs active
  if(!max30105.init(2)) {
    printf("MAX30105: Failed to initialize. %02x\n", max30105.get_chip_id());
  }

  // Configure red LED to show as an "on" indicator
  max30105.set_slot_mode(1, red);
  max30105.set_led_pulse_amplitude(1, 0.2);

  // Configure IR LED to detect pulse
  max30105.set_slot_mode(2, ir);
  max30105.set_led_pulse_amplitude(2, 12.5);

  // Turn off LEDs 3 & 4
  max30105.set_slot_mode(3, off);
  max30105.set_led_pulse_amplitude(3, 0);
  max30105.set_slot_mode(4, off);

  while(true) {
    uint32_t samples[32 * 3];
    int num_samples = max30105.get_samples(samples);
    float temperature = max30105.get_temperature();

    bool beat = false;
    
    for(auto x = 0u; x < num_samples; x+=2) {
      if(checkForBeat(samples[x+1])) {
        beat = true;
      }
    }

    // Flash LED if a "beat" is detected
    gpio_put(PICO_DEFAULT_LED_PIN, beat);

    /*printf("MAX30105: Samples: %i %f\n", num_samples);
    printf("MAX30105: Temperature: %fC\n", temperature);
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(500);*/
  }

  return 0;
}
