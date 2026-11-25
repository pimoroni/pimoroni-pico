#include "pico/stdlib.h"
#include <stdio.h>

#include "common/pimoroni_common.hpp"
#include "badger2040w.hpp"

#include "badger2040w_image_demo_images.hpp"

using namespace pimoroni;

Badger2040W badger;

int main() {

  badger.init();
  stdio_init_all();

  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.uc8151->set_update_speed(2);


  if(badger.pressed_to_wake(badger.A)) {
    printf("> A pressed\n");
    badger.image(shaun, Rect(0, 0, badger.DISPLAY_WIDTH, badger.DISPLAY_HEIGHT));
  }

  else if(badger.pressed_to_wake(badger.B)) {
    printf("> B pressed\n");
    badger.image(paul, Rect(0, 0, badger.DISPLAY_WIDTH, badger.DISPLAY_HEIGHT));
  }

  else if(badger.pressed_to_wake(badger.C)) {
    printf("> C pressed\n");
    badger.image(adam, Rect(0, 0, badger.DISPLAY_WIDTH, badger.DISPLAY_HEIGHT));
  }

  else {
    printf("> No A/B/C key pressed\n");
    badger.graphics->set_pen(15);
    badger.graphics->clear();

    badger.graphics->set_pen(0);
    badger.graphics->set_font("sans");
    badger.graphics->text("Press A, B, or C", Point(15, 65), badger.DISPLAY_WIDTH, 1.0f);
  }

  badger.update();
  badger.halt();

}
