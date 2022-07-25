#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#include "okcolor.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB565 graphics(53, 11, nullptr);
GalacticUnicorn galactic_unicorn;

std::string message = "Pirate. Monkey. Robot. Ninja.";

int main() {

  stdio_init_all();

  galactic_unicorn.init();

  float scroll = -53.0f;

  while(true) {
    //uint time_ms = to_ms_since_boot(get_absolute_time());

    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      galactic_unicorn.adjust_brightness(+0.01);
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      galactic_unicorn.adjust_brightness(-0.01);
    }

    int width = graphics.measure_text(message, 1);
    scroll += 0.25f;

    if(scroll > width) {
      scroll = -53.0f;
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();
    
    ok_color::HSL hsl{scroll / 100.0f, 1.0f, 0.5f};
    ok_color::RGB rgb = ok_color::okhsl_to_srgb(hsl);
    graphics.set_pen(rgb.r * 255, rgb.g * 255, rgb.b * 255);
    graphics.text(message, Point(0 - scroll, 5), -1, 0.55);
    /*graphics.text(message, Point(0 - scroll + 1, 5), -1, 0.5);
    graphics.text(message, Point(0 - scroll, 5 + 1), -1, 0.5);
    graphics.text(message, Point(0 - scroll + 1, 5 + 1), -1, 0.5);*/
/*
    for(int x = 0; x < 53; x++) {
      for(int y = 0; y < 2; y++) {
        float b = sin((x - y * 3 + int(scroll) + 100) / 3.0f);
        graphics.set_pen(180 * b, 150 * b, 0);
        graphics.pixel(Point(52 - x, y));
        graphics.pixel(Point(52 - x, y + 9));
      }
    }*/

    galactic_unicorn.update(graphics);
    
    sleep_ms(10);
  }

  return 0;
}
