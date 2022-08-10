#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "badger2040.hpp"

using namespace pimoroni;

Badger2040 badger;

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
  badger.pen(15);
  badger.clear();

  badger.font("sans");
  for(int i = 0; i < int(font_names.size()); i++) {
    std::string name = font_names[i];

    if(selected_font == i) {
      badger.pen(0);
      badger.rectangle(0, i * 16, 80, 16);
      badger.pen(15);
    }else{
      badger.pen(0);
    }

    badger.text(name, 2, i * 16 + 7, 0.4f);
  }

  badger.font(font_names[selected_font]);
  badger.thickness(2);
  badger.text("The quick", 90, 10, 0.80f);
  badger.text("brown fox", 90, 32, 0.80f);
  badger.text("jumped over", 90, 54, 0.80f);
  badger.text("the lazy dog.", 90, 76, 0.80f);
  badger.text("0123456789", 90, 98, 0.80f);
  badger.text("!\"£$%^&*()", 90, 120, 0.80f);
  badger.thickness(1);

  badger.update();
}

int main() {

  stdio_init_all();

  sleep_ms(500);

  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.init();
  badger.update_speed(1);

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
