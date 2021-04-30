#include <math.h>

#include "breakout_dotmatrix.hpp"
#include "time.h"

using namespace pimoroni;

BreakoutDotMatrix display;
bool led_toggle = false;

void eye(uint8_t x, uint8_t y) {
  display.set_pixel(x, y, true);
  display.set_pixel(x, y + 1, true);
  display.set_pixel(x + 1, y, true);
  display.set_pixel(x + 1, y + 1, true);
}

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  display.init();

  while(true) {
    absolute_time_t at = get_absolute_time();
    uint64_t t = to_us_since_boot(at) / 1000000;

    // Get eye x and y positions in the range 0.0 to 1.0
    // You can plug in your own 0.0 to 1.0 values here
    uint64_t t_pi = t * M_PI;
    float x = (sinf(t_pi / 2) + 1) / 2;
    float y = (sinf(t_pi / 4) + 1) / 2;

    // Multiply them up to display coords and convert to int
    x = floor(x * 3);
    y = floor(y * 5);

    // Clear the display
    display.clear();

    // Blink occasionally
    if(!(rand() % 20 == 2)) {
        // Draw the eyes if not blinking
        eye(x, y);
        eye(x + 5, y);
    }

    // Update the display at 10FPS
    // This gives our crude blink code time to not look like a random flicker
    display.show();

    gpio_put(PICO_DEFAULT_LED_PIN, led_toggle);
    led_toggle = !led_toggle;

    sleep_ms(100);
  }

  return 0;
}