#include "pico/stdlib.h"

#include "breakout_tof.hpp"

using namespace pimoroni;

BreakoutTOF tof;

int main() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  bool tof_present = tof.init();
  if(!tof_present) {
    printf("VL53L1X not found!\n");
    return 0;
  }

  printf("VL53L1X found...\n");

  tof.start_continuous(1000);
  tof.set_measurement_timing_budget(50000);
  tof.set_distance_mode_int(1);

  while(true) {
    uint16_t distance = tof.read(true);
    printf("Distance: %imm\n", distance);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(500);
  }

  return 0;
}
