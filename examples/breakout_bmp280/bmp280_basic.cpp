#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "bmp280.hpp"
#include "common/pimoroni_i2c.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BMP280 bmp280(&i2c);


int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  stdio_init_all();

  if(!bmp280.init()) {
    printf("Failed to init bmp280!\n");
  }

  while (1) {
    BMP280::bmp280_reading result = bmp280.read();
    printf("%s %0.2lf deg C, %ld hPa\n", result.status == BMP280_OK ? "OK" : "ER", result.temperature, result.pressure);
    sleep_ms(1000);
  }

  return 0;
}