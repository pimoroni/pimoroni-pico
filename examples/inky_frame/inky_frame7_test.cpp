#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "drivers/psram_display/psram_display.hpp"
#include "drivers/inky73/inky73.hpp"

using namespace pimoroni;

int main() {
  stdio_init_all();

  PSRamDisplay ramDisplay(800, 480);
  PicoGraphics_PenInky7 graphics(800, 480, ramDisplay);
  UC8159Inky7 uc8159(800,400);

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
  
  uc8159.update(&graphics);

  while(true)
    sleep_ms(1000);

  return 0;
}
