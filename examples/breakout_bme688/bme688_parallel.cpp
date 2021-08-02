#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "bme68x.hpp"
#include "common/pimoroni_i2c.hpp"

/*
Read a sequence of readings from the BME688 with given heat/duration profiles
Reading the full batch of readings will take some time. This seems to take ~10sec.
*/

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BME68X bme68x(&i2c);

constexpr uint16_t profile_length = 10;

// Space for <profile_length> results
bme68x_data data[profile_length];

/* Heater temperature in degree Celsius */
uint16_t temps[profile_length] = { 320, 100, 100, 100, 200, 200, 200, 320, 320, 320 };

/* Multiplier to the shared heater duration */
uint16_t durations[profile_length] = { 5, 2, 10, 30, 5, 5, 5, 5, 5, 5 };


int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  stdio_init_all();

  bme68x.init();

  while (1) {
    sleep_ms(1000);
    auto time_start = get_absolute_time();

    printf("Fetching %u readings, please wait...\n", profile_length);

    auto result = bme68x.read_parallel(data, temps, durations, profile_length);
    (void)result;

    auto time_end = get_absolute_time();
    auto duration = absolute_time_diff_us(time_start, time_end);
    auto time_ms = to_ms_since_boot(time_start);

    printf("Done at %lu in %lluus\n", (long unsigned int)time_ms, (long long unsigned int)duration);

    for(auto i = 0u; i < 10u; i++){
      printf("%d, %d: %.2f, %.2f, %.2f, %.2f, 0x%x\n",
          data[i].gas_index,
          data[i].meas_index,
          data[i].temperature,
          data[i].pressure,
          data[i].humidity,
          data[i].gas_resistance,
          data[i].status);
    }
  }

  return 0;
}
