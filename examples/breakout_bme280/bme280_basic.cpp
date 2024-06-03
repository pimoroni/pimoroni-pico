#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "bme280.hpp"
#include "common/pimoroni_i2c.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BME280 bme280(&i2c);


int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  if(!bme280.init()) {
    printf("Failed to init bme280!\n");
  }

  while (1) {
    BME280::bme280_reading result = bme280.read_forced();
    printf("%s %0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", result.status ? "OK" : "ER", result.temperature, result.pressure, result.humidity);
    sleep_ms(1000);
  }

  return 0;
}