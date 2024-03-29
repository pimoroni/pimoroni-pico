#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "stellar_unicorn.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(16, 16, nullptr);
StellarUnicorn stellar_unicorn;

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
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

void text(std::string t, Point p, float s = 1.0f, float a = 1.0f) {
  int w = graphics.measure_text(t, s);
  p.x += (16 / 2) - (w / 2);
  p.y += (16 / 2);
  graphics.text(t, Point(p.x, p.y), -1, s, a);
  graphics.text(t, Point(p.x + 1, p.y), -1, s, a);
  graphics.text(t, Point(p.x + 1, p.y + 1), -1, s, a);
  graphics.text(t, Point(p.x, p.y + 1), -1, s, a);
}


int main() {
  uint8_t hue_map[16][3];
  for(int i = 0; i < 16; i++) {
    from_hsv(i / 16.0f, 1.0f, 0.5f, hue_map[i][0], hue_map[i][1], hue_map[i][2]);
  }

  stellar_unicorn.init();

  graphics.set_font("sans");
  uint i = 0;

  while(true) {
 
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_UP)) {
      stellar_unicorn.adjust_brightness(+0.01);
    }
    if(stellar_unicorn.is_pressed(stellar_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      stellar_unicorn.adjust_brightness(-0.01);
    }

    i++;
    
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    float s = 0.4f;//0.65f + (sin(i / 25.0f) * 0.15f);
    float a = 1.0f;// (sin(i / 25.0f) * 100.0f);

    float x = (sin((i) / 50.0f) * 45.0f);
    float y = (cos((i) / 40.0f) * 2.5f);
    graphics.set_pen(255, 255, 255);
    text("Galactic Unicorn", Point(x, y), s, a);
    uint8_t *p = (uint8_t *)graphics.frame_buffer;
    for(size_t i = 0; i < 16 * 16; i++) {
      int x = i % 16;
      int y = i / 16;
      uint r = *p++;
      uint g = *p++;
      uint b = *p++;
      p++;

      if(r > 0) {
        r = hue_map[x][0];
        g = hue_map[x][1];
        b = hue_map[x][2]; 
      }

      graphics.set_pen(r, g, b);
      graphics.pixel(Point(x, y));
    }

    stellar_unicorn.update(&graphics);
  }

  printf("done\n");

  return 0;
}
