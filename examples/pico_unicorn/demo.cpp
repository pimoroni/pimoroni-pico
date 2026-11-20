#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn<14,1,0, uint16_t, pimoroni::GAMMA_14BIT> pico_unicorn;

int main() {
  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;

  uint32_t i = 0;
  while(true) {
    i = i + 1;

    if(pico_unicorn.is_pressed(pico_unicorn.A)) { a_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.B)) { b_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.X)) { x_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.Y)) { y_pressed = true; }

    pico_unicorn.clear();

    if(a_pressed & b_pressed & x_pressed & y_pressed) {
      for(int y = 0; y < 7; y++) {
        for(int x = 0; x < 16; x++) {
          int v = (x + y + (i / 100)) % 2 == 0 ? 0 : 100;
          pico_unicorn.set_pixel(x, y, v);
        }
      }
    } else {
      // button test phase
      float pulse = fmod(float(i) / 20.0f, M_PI * 2.0f);
      int v = int((sin(pulse) * 50.0f) + 50.0f);
      if(a_pressed) {
      }else{
        pico_unicorn.set_pixel(0, 0, v);
        pico_unicorn.set_pixel(0, 1, v);
        pico_unicorn.set_pixel(1, 0, v);
        pico_unicorn.set_pixel(1, 1, v);
        pico_unicorn.set_pixel(1, 2, v / 2);
        pico_unicorn.set_pixel(0, 2, v / 2);
        pico_unicorn.set_pixel(2, 0, v / 2);
        pico_unicorn.set_pixel(2, 1, v / 2);
      }

      if(b_pressed) {
      }else{
        pico_unicorn.set_pixel(0, 6, v);
        pico_unicorn.set_pixel(0, 5, v);
        pico_unicorn.set_pixel(1, 6, v);
        pico_unicorn.set_pixel(1, 5, v);
        pico_unicorn.set_pixel(1, 4, v / 2);
        pico_unicorn.set_pixel(0, 4, v / 2);
        pico_unicorn.set_pixel(2, 6, v / 2);
        pico_unicorn.set_pixel(2, 5, v / 2);
      }

      if(x_pressed) {
      }else{
        pico_unicorn.set_pixel(15, 0, v);
        pico_unicorn.set_pixel(15, 1, v);
        pico_unicorn.set_pixel(14, 0, v);
        pico_unicorn.set_pixel(14, 1, v);
        pico_unicorn.set_pixel(14, 2, v / 2);
        pico_unicorn.set_pixel(15, 2, v / 2);
        pico_unicorn.set_pixel(13, 0, v / 2);
        pico_unicorn.set_pixel(13, 1, v / 2);
      }

      if(y_pressed) {
      }else{
        pico_unicorn.set_pixel(15, 6, v);
        pico_unicorn.set_pixel(15, 5, v);
        pico_unicorn.set_pixel(14, 6, v);
        pico_unicorn.set_pixel(14, 5, v);
        pico_unicorn.set_pixel(14, 4, v / 2);
        pico_unicorn.set_pixel(15, 4, v / 2);
        pico_unicorn.set_pixel(13, 6, v / 2);
        pico_unicorn.set_pixel(13, 5, v / 2);
      }
    }

    sleep_ms(10);

  }

  printf("done\n");

  return 0;
}
