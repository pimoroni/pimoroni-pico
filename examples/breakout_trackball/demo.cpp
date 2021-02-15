#include "pico/stdlib.h"

#include "breakout_trackball.hpp"

using namespace pimoroni;

Trackball trackball;

int main() {
  setup_default_uart();

  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  

  trackball.init();

  trackball.set_white(255);

  while(true) {
    trackball.set_rgbw(255, 0, 0, 0);
    sleep_ms(1000);
    trackball.set_rgbw(255, 255, 0, 0);
    sleep_ms(1000);
    trackball.set_rgbw(0, 255, 0, 0);
    sleep_ms(1000);
    trackball.set_rgbw(0, 255, 255, 0);
    sleep_ms(1000);
    trackball.set_rgbw(0, 0, 255, 0);
    sleep_ms(1000);
    trackball.set_rgbw(0, 0, 255, 255);
    sleep_ms(1000);
    trackball.set_rgbw(0, 0, 0, 255);
    sleep_ms(1000);
    trackball.set_rgbw(255, 0, 0, 255);
    sleep_ms(1000);
  }

  return 0;
}
