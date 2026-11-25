#include "pico/stdlib.h"
#include <stdio.h>

#include "common/pimoroni_common.hpp"
#include "badger2040w.hpp"

using namespace pimoroni;

Badger2040W badger;

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

int main() {

  badger.init();
  stdio_init_all();
  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.uc8151->set_update_speed(2);
  badger.graphics->set_font("bitmap8");
  badger.graphics->set_thickness(2);
  char time_str[11];
  while(true) {

    badger.graphics->set_pen(15);
    badger.graphics->clear();

    badger.graphics->set_pen(0);
    badger.graphics->rectangle(Rect(0,badger.DISPLAY_HEIGHT / 4,badger.DISPLAY_WIDTH,badger.DISPLAY_HEIGHT / 2));
    badger.graphics->set_pen(15);
    badger.graphics->rectangle(Rect(5,badger.DISPLAY_HEIGHT / 4 + 5,badger.DISPLAY_WIDTH - 10,badger.DISPLAY_HEIGHT / 2 - 10));

    badger.graphics->set_pen(0);
    sprintf(time_str, "%ld", time());
    int32_t time_x_centered = (badger.DISPLAY_WIDTH - badger.graphics->measure_text(time_str, 2.0f)) / 2;
    badger.graphics->text("Time since boot (ms)", Point(5, 10), badger.DISPLAY_WIDTH, 2.0f);
    badger.graphics->text(time_str, Point(time_x_centered, badger.DISPLAY_HEIGHT / 2 - 8), badger.DISPLAY_WIDTH, 2.0f);
      
    badger.update();
    sleep_ms(5000);
  }
}
