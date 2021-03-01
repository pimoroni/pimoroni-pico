#include "pico/stdlib.h"

#include "breakout_matrix11x7.hpp"

using namespace pimoroni;

static const uint8_t ON_BRIGHTNESS = 64;


BreakoutMatrix11x7 matrix11x7;

int main() {
  matrix11x7.init();

  int x = 0;
  int y = 0;
  bool light = true;

  while(1) {
      matrix11x7.set_pixel(x, y, light ? ON_BRIGHTNESS : 0);
      matrix11x7.update();

      x++;
      if(x >= BreakoutMatrix11x7::WIDTH) {
          x = 0;
          y++;
          if(y >= BreakoutMatrix11x7::HEIGHT) {
              y = 0;
              light = !light;
              sleep_ms(500);
          }
      }
      sleep_ms(10);
  }

  return 0;
}
