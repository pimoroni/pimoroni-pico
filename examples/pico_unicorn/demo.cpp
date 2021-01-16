#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      case 5: r = v; g = p; b = q; break;
    }
  }


int main() {

  pico_unicorn.init();

  uint32_t i = 0;
  while(true) {
    i = i + 1;
    uint8_t j = 0;
    for(uint8_t y = 0; y < 7; y++) {
      for(uint8_t x = 0; x < 16; x++) {
        uint8_t r, g, b;
        float h = float(x) / 63.0f + float(i) / 500.0f;
        h = h - float(int(h));
        float s = 1.0f;//(sin(float(i) / 200.0f) * 0.5f) + 0.5f;
        float v = (float(y) / 8.0f) + 0.05f;
        from_hsv(h, s, v, r, g, b);

        pico_unicorn.set_pixel(x, y, r, g, b);
        j = j + 1;
      }
    }

    pico_unicorn.update();
  }

  printf("done\n");

  return 0;
}
