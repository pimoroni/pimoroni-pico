#include <math.h>
#include <string>

#include "breakout_dotmatrix.hpp"
#include "time.h"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutDotMatrix display(&i2c);
bool led_toggle = false;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  display.init();
  display.show();

  while(true) {
    absolute_time_t at = get_absolute_time();
    uint64_t t = to_us_since_boot(at) / 1000000;

    std::string second = std::to_string(t % 60);

    uint16_t left, right;
    if(second.length() == 1) {
        left = '0';
        right = second[0];
    }
    else {
        left = second[0];
        right = second[1];
    }

    display.set_character(0, left);
    display.set_character(5, right);
    display.show();

#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, led_toggle);
#endif
    led_toggle = !led_toggle;

    sleep_ms(1000 / 60);
  }

  return 0;
}
