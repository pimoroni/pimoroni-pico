#include "pico/stdlib.h"
#include <stdio.h>

#include "common/pimoroni_common.hpp"
#include "badger2040w.hpp"

#include "badger2040w_icon_demo_icons.hpp"

using namespace pimoroni;

Badger2040W badger;

int main() {

  badger.init();
  stdio_init_all();

  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.uc8151->set_update_speed(1);

  badger.graphics->set_pen(15);
  badger.graphics->clear();
  auto iconsize = 40;
  auto x_offset = 10;
  for (auto x=0; x < (int)badger.DISPLAY_WIDTH / iconsize; x++) {
    for (auto i=0; i < 3; i++) {
      auto idx = (i + x) % 3;
      badger.icon(iconsheet, idx, iconsize * 3, Rect(x_offset + x * iconsize, iconsize * i, iconsize, iconsize));
    }
  }

  badger.update();
  badger.halt();

}
