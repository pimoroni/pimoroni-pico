#include <math.h>

#include "breakout_dotmatrix.hpp"
#include "time.h"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutDotMatrix display(&i2c);
static const uint8_t WIDTH = display.WIDTH;
static const uint8_t HEIGHT = display.HEIGHT;
static const uint8_t HALF_WIDTH = (WIDTH / 2);

uint8_t values[HALF_WIDTH] = { 0, 0, 0, 0, 0 };
uint8_t next_value = 0;

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  display.init();

  while(true) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);

    absolute_time_t at = get_absolute_time();
    uint64_t t = to_us_since_boot(at) / 1000000;

    // Add a new random value to our array, overriding the oldest value
    values[next_value] = rand() % HEIGHT;
    next_value = (next_value + 1) % HALF_WIDTH;

    // Animate a value from 0 to height + 1
    float value = (sinf(t * M_PI) + 1) / 2.0f;
    value *= HEIGHT + 1;
    value = floor(value);

    for(uint8_t y = 0; y < HEIGHT; y++) {
      uint8_t y_inv = HEIGHT - 1 - y;

      for(uint8_t x = 0; x < HALF_WIDTH; x++) {
        // Left
        display.set_pixel(x, y_inv, value <= y_inv);

        // Right
        display.set_pixel(x + HALF_WIDTH, y_inv, values[(x + next_value) % HALF_WIDTH] <= y_inv);
      }
    }

    display.show();

    gpio_put(PICO_DEFAULT_LED_PIN, false);

    sleep_ms(1000 / HEIGHT);
  }

  return 0;
}