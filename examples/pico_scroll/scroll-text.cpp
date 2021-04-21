#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_scroll.hpp"

using namespace pimoroni;

PicoScroll pico_scroll;

int main() {
  stdio_init_all();
  pico_scroll.init();

  while(true) {
    pico_scroll.scroll_text("Hello World, how are you today?", 64, 100);
    sleep_ms(500);

    pico_scroll.set_text("Test", 64);
    pico_scroll.update();
    sleep_ms(1000);

    // Set pixels to a chessboard pattern
    char pixels[PicoScroll::WIDTH * PicoScroll::HEIGHT] = {};

    pico_scroll.set_pixels(pixels);
    for (int y = 0; y < PicoScroll::HEIGHT; y++) {
        for (int x = 0; x < PicoScroll::WIDTH; x++) {
            pixels[y * PicoScroll::WIDTH + x] = ((x + y) & 0b1) * 64;
        }
    }

    pico_scroll.set_pixels(pixels);
    pico_scroll.update();

    sleep_ms(1000);
  }

  return 0;
}
