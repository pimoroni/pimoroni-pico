#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_scroll.hpp"

using namespace pimoroni;

PicoScroll pico_scroll;

int main() {

  pico_scroll.init();


  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;

  uint32_t i = 0;
  while(true) {
    i++;

    if(pico_scroll.is_pressed(pico_scroll.A)) { a_pressed = true; }
    if(pico_scroll.is_pressed(pico_scroll.B)) { b_pressed = true; }
    if(pico_scroll.is_pressed(pico_scroll.X)) { x_pressed = true; }
    if(pico_scroll.is_pressed(pico_scroll.Y)) { y_pressed = true; }

    pico_scroll.clear();

    if(a_pressed & b_pressed & x_pressed & y_pressed) {
      for(int y = 0; y < 7; y++) {
        for(int x = 0; x < 17; x++) {
          int v = (x + y + (i / 25)) % 2 == 0 ? 0 : 3;
          pico_scroll.set_pixel(x, y, v);
        }
      }
    } else {
      // button test phase
      float pulse = fmod(float(i) / 5.0f, M_PI * 2.0f);
      int v = int((sin(pulse) * 4.0f) + 4.0f);
      if(a_pressed) {
      }else{
        pico_scroll.set_pixel(0, 0, v);
        pico_scroll.set_pixel(0, 1, v);
        pico_scroll.set_pixel(1, 0, v);
        pico_scroll.set_pixel(1, 1, v);
        pico_scroll.set_pixel(1, 2, v / 2);
        pico_scroll.set_pixel(0, 2, v / 2);
        pico_scroll.set_pixel(2, 0, v / 2);
        pico_scroll.set_pixel(2, 1, v / 2);
      }

      if(b_pressed) {
      }else{
        pico_scroll.set_pixel(0, 6, v);
        pico_scroll.set_pixel(0, 5, v);
        pico_scroll.set_pixel(1, 6, v);
        pico_scroll.set_pixel(1, 5, v);
        pico_scroll.set_pixel(1, 4, v / 2);
        pico_scroll.set_pixel(0, 4, v / 2);
        pico_scroll.set_pixel(2, 6, v / 2);
        pico_scroll.set_pixel(2, 5, v / 2);
      }

      if(x_pressed) {
      }else{
        pico_scroll.set_pixel(16, 0, v);
        pico_scroll.set_pixel(16, 1, v);
        pico_scroll.set_pixel(15, 0, v);
        pico_scroll.set_pixel(15, 1, v);
        pico_scroll.set_pixel(15, 2, v / 2);
        pico_scroll.set_pixel(16, 2, v / 2);
        pico_scroll.set_pixel(14, 0, v / 2);
        pico_scroll.set_pixel(14, 1, v / 2);
      }

      if(y_pressed) {
      }else{
        pico_scroll.set_pixel(16, 6, v);
        pico_scroll.set_pixel(16, 5, v);
        pico_scroll.set_pixel(15, 6, v);
        pico_scroll.set_pixel(15, 5, v);
        pico_scroll.set_pixel(15, 4, v / 2);
        pico_scroll.set_pixel(16, 4, v / 2);
        pico_scroll.set_pixel(14, 6, v / 2);
        pico_scroll.set_pixel(14, 5, v / 2);
      }
    }

    pico_scroll.update();

    sleep_ms(10);
  }

  return 0;
}
