#include "pico/stdlib.h"

#include "breakout_matrix11x7.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutMatrix11x7 matrix11x7(&i2c, 0x75);

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  matrix11x7.init();

  while(true) {
    for(auto x = 0; x < matrix11x7.WIDTH; x++) {
      for(auto y = 0; y < matrix11x7.HEIGHT; y++) {
        matrix11x7.set_pixel(x, y, ((x + y) & 0b1) * 128);
      }
    }
    matrix11x7.update(0);
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(1000);
    for(auto x = 0; x < matrix11x7.WIDTH; x++) {
      for(auto y = 0; y < matrix11x7.HEIGHT; y++) {
        matrix11x7.set_pixel(x, y, ((x + y + 1) & 0b1) * 128);
      }
    }
    matrix11x7.update(0);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(1000);
  }

  return 0;
}
