#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "badger2040w.hpp"

using namespace pimoroni;

Badger2040W badger;

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

std::array<std::string, 8> font_names = {
  "sans", "sans_bold", "gothic", "cursive",
  "cursive_bold", "serif", "serif_bold", "serif_italic"
};
int8_t selected_font = 0;

void draw() {
  badger.graphics->set_pen(15);
  badger.graphics->clear();

  badger.graphics->set_font("sans");
  for(int i = 0; i < int(font_names.size()); i++) {
    std::string name = font_names[i];

    if(selected_font == i) {
      badger.graphics->set_pen(0);
      badger.graphics->rectangle(Rect(0, i * 16, 80, 16));
      badger.graphics->set_pen(15);
    }else{
      badger.graphics->set_pen(0);
    }

    badger.graphics->text(name, Point(2, i * 16 + 7), badger.DISPLAY_WIDTH, 0.4f);
  }

  badger.graphics->set_font(font_names[selected_font]);
  badger.graphics->set_thickness(2);
  badger.graphics->text("The quick", Point(90, 10), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->text("brown fox", Point(90, 32), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->text("jumped over", Point(90, 54), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->text("the lazy dog.", Point(90, 76), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->text("0123456789", Point(90, 98), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->text("!\"£$%^&*()", Point(90, 120), badger.DISPLAY_WIDTH, 0.80f);
  badger.graphics->set_thickness(1);

  badger.update();
}

int main() {

  badger.init();
  stdio_init_all();

  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.uc8151->set_update_speed(2);

  uint32_t i = 0;

  while(true) {
    printf("> drawing..");

    draw();

    printf("done!\n");

    printf("> waiting for a button press..");
    badger.wait_for_press();
    printf("done!\n");

    if(badger.pressed(badger.DOWN)) {
      printf("> down pressed\n");
      selected_font++;
    }

    if(badger.pressed(badger.UP)) {
      printf("> up pressed\n");
      selected_font--;
    }

    if(badger.pressed(badger.C)) {
      printf("> C pressed\n");
      badger.halt();
    }

    selected_font = selected_font < 0 ? int(font_names.size()) - 1 : selected_font;
    selected_font = selected_font >= int(font_names.size()) ? 0 : selected_font;

    printf("> newly selected font is %s (%d)\n", font_names[selected_font].c_str(), selected_font);

    i++;
  }
}
