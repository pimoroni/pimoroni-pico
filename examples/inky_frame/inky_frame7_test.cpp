#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "drivers/psram_display/psram_display.hpp"
#include "drivers/inky73/inky73.hpp"

using namespace pimoroni;

uint LED_PIN = 8;

int main() {
  stdio_init_all();

  gpio_init(LED_PIN);
  gpio_set_function(LED_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  PSRamDisplay ramDisplay(800, 480);
  PicoGraphics_PenInky7 graphics(800, 480, ramDisplay);
  UC8159Inky7 inky7(800,400);

  while (true) {
    while(!inky7.is_pressed(UC8159Inky7::BUTTON_A)) {
      sleep_ms(10);
    }
    graphics.set_pen(1);
    graphics.clear();

    for(int i =0 ; i < 100 ; i++)
    {
      uint size = 25 + (rand() % 50);
      uint x = rand() % graphics.bounds.w;
      uint y = rand() % graphics.bounds.h;

      graphics.set_pen(0);
      graphics.circle(Point(x, y), size);

      graphics.set_pen(2+(i%5));
      graphics.circle(Point(x, y), size-2);
    }
  
    gpio_put(LED_PIN, 1);
    inky7.update(&graphics);
    gpio_put(LED_PIN, 0);
  }

  return 0;
}
