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

std::array<std::string, 11> c64 = {
  "",
  " OOOOO   OOOOOO    OO    OOOO    OO  OO      XXXXXXX",
  " OO  OO  OO       OOOO   OO OO   OO  OO      XXXXXXX",
  " OO  OO  OO      OO  OO  OO  OO  OO  OO      XXXXXXX",
  " OOOOO   OOOO    OOOOOO  OO  OO   OOOO       XXXXXXX",
  " OOOO    OO      OO  OO  OO  OO    OO        XXXXXXX",
  " OO OO   OO      OO  OO  OO OO     OO    OO  XXXXXXX",
  " OO  OO  OOOOOO  OO  OO  OOOO      OO    OO  XXXXXXX",
  "                                             XXXXXXX"
};

std::array<std::string, 11> spectrum = {
  "",
  " O        OOOO    OOOO   OOOOO     O O  O O XXXXXXXX",
  " O       O    O  O    O  O    O    O O  O O X XXXXXX",
  " O       O    O  O    O  O    O             X XXXXXX",
  " O       O    O  OOOOOO  O    O             X XXXXXX",
  " O       O    O  O    O  O    O             X XXXXXX",
  " OOOOOO   OOOO   O    O  OOOOO              X XXXXXX",
  "                                            X      X",
  "                                            XXXXXXXX"
};

std::array<std::string, 11> bbc_micro = {
  "",
  " OOOOO    OO    OOOO   OOO    OOOO      O           ",
  " O    O  O  O  O    O   O    O    O      O          ",
  " O    O O    O O        O    O            O         ",
  " OOOOO  O    O  OOOO    O    O             O        ",
  " O    O OOOOOO      O   O    O            O         ",
  " O    O O    O O    O   O    O    O      O          ",
  " OOOOO  O    O  OOOO   OOO    OOOO      O           ",
  "                                             XXXXXXX"
};


constexpr uint PROMPT_C64 = 0;
constexpr uint PROMPT_SPECTRUM = 1;
constexpr uint PROMPT_BBC_MICRO = 2;
uint prompt = 0;

int main() {

  stdio_init_all();

  galactic_unicorn.init();

  while(true) {
    uint time_ms = to_ms_since_boot(get_absolute_time());

    prompt = (time_ms / 3000) % 3;

    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      galactic_unicorn.adjust_brightness(+0.01);
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      galactic_unicorn.adjust_brightness(-0.01);
    }

    std::array<std::string, 11> image = c64;
    if(prompt == PROMPT_C64) {
      image = c64;
    }else if(prompt == PROMPT_SPECTRUM) {
      image = spectrum;
    }else if(prompt == PROMPT_BBC_MICRO) {
      image = bbc_micro;
    }

    if(prompt == PROMPT_C64) {
      graphics.set_pen(20, 20, 120);
    }else if(prompt == PROMPT_SPECTRUM){
      graphics.set_pen(180, 150, 150);
    }else if(prompt == PROMPT_BBC_MICRO){
      graphics.set_pen(0, 0, 0);
    }
    
    graphics.clear();

    if(prompt == PROMPT_C64) {
      graphics.set_pen(230, 210, 250);
    }else if(prompt == PROMPT_SPECTRUM){
      graphics.set_pen(0, 0, 0);
    }else if(prompt == PROMPT_BBC_MICRO){
      graphics.set_pen(255, 255, 255);
    }

    for(size_t y = 0; y < image.size(); y++) {
      for(size_t x = 0; x < image[y].length(); x++) {
        // draw the prompt text
        if(image[y][x] == 'O') {
          graphics.pixel(Point(x, y + 1));    
        }

        // draw the caret blinking
        if(image[y][x] == 'X' && (time_ms / 300) % 2) {
          graphics.pixel(Point(x, y + 1));    
        }
      }
    }

    galactic_unicorn.update(graphics);
    
    sleep_ms(10);
  }

  return 0;
}
