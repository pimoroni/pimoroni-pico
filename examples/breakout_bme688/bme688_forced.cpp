#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "bme68x.hpp"
#include "common/pimoroni_i2c.hpp"

/*
Read a single reading from the BME688
*/

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BME68X bme68x(&i2c);

int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  bme68x.init();

  while (1) {
    sleep_ms(1000);
    auto time_abs = get_absolute_time();
    auto time_ms = to_ms_since_boot(time_abs);

    bme68x_data data;

    auto result = bme68x.read_forced(&data);
    (void)result;

    printf("%lu, %.2f, %.2f, %.2f, %.2f, 0x%x, %d, %d\n",
        (long unsigned int)time_ms,
        data.temperature,
        data.pressure,
        data.humidity,
        data.gas_resistance,
        data.status,
        data.gas_index,
        data.meas_index);
  }

  return 0;
}
