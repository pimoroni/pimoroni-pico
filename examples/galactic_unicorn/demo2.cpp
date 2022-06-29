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
  p.x += (53 / 2) - (w / 2);
  p.y += (11 / 2);
  graphics.text(t, Point(p.x, p.y), -1, s, a);
  graphics.text(t, Point(p.x + 1, p.y), -1, s, a);
  graphics.text(t, Point(p.x + 1, p.y + 1), -1, s, a);
  graphics.text(t, Point(p.x, p.y + 1), -1, s, a);
}

struct star_t {
  float dx, dy, x, y, a;

  uint8_t brightness() {
    int b = a / 5;
    return b > 15 ? 15 : b;
  }
};

void init_star(star_t &s) {
  s.x = ((rand() % 100) / 5.0f) - 10.0f;
  s.y = ((rand() % 100) / 10.0f) - 5.0f;

  s.dx = s.x / 10.0f;
  s.dy = s.y / 10.0f;
  s.a = 0;
}

void step_star(star_t &s) {
  s.x += s.dx;
  s.y += s.dy;
  s.a++;

  if(s.a > 100) {
    init_star(s);
  }
}

void number(int n, int x, int y) {
  switch(n) {
    case 6: {
      graphics.rectangle(Rect(x, y, 6, 2));
      graphics.rectangle(Rect(x, y + 4, 6, 2));
      graphics.rectangle(Rect(x, y + 9, 6, 2));
      graphics.rectangle(Rect(x, y, 2, 11));
      graphics.rectangle(Rect(x + 4, y + 4, 2, 5));
    }break;

    case 9: {
      graphics.rectangle(Rect(x, y, 6, 2));
      graphics.rectangle(Rect(x, y + 4, 6, 2));
      graphics.rectangle(Rect(x, y + 9, 6, 2));
      graphics.rectangle(Rect(x + 4, y, 2, 11));
      graphics.rectangle(Rect(x, y, 2, 5));
    }break;

    case 4: {
      graphics.rectangle(Rect(x, y + 4, 6, 2));
      graphics.rectangle(Rect(x + 4, y, 2, 11));
      graphics.rectangle(Rect(x, y, 2, 5));
    }break;

    case 0: {
      graphics.rectangle(Rect(x, y, 6, 2));
      graphics.rectangle(Rect(x + 4, y, 2, 11));
      graphics.rectangle(Rect(x, y, 2, 11));
      graphics.rectangle(Rect(x, y + 9, 6, 2));
    }break;
  }
}

int main() {

  galactic_unicorn.init();

  // graphics.set_font("sans");


  while(true) {
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_VOLUME_UP)) {
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_VOLUME_DOWN)) {
    }

    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
    }

    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_A)) {
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_B)) {
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();
    graphics.set_pen(200, 200, 0);
    number(6, 23, 0);
    number(9, 31, 0);
    number(4, 39, 0);
    number(0, 47, 0);

    graphics.set_pen(200, 0, 0);
    graphics.circle(Point(8, 5), 6);

    graphics.set_pen(255, 255, 255);
    std::vector<Point> triangle = {Point(5, 2), Point(10, 5), Point(5, 7)};
    graphics.polygon(triangle);


    graphics.set_pen(255, 0, 0);
    graphics.rectangle(Rect(0, 0, 10, 11));
    graphics.set_pen(0, 255, 0);
    graphics.rectangle(Rect(10, 0, 10, 11));
    graphics.set_pen(0, 0, 255);
    graphics.rectangle(Rect(20, 0, 10, 11));

    uint16_t *p = (uint16_t *)graphics.frame_buffer;
    for(size_t i = 0; i < 53 * 11; i++) {
      int x = i % 53;
      int y = i / 53;
     /* uint r = ((*p & 0b0111110000000000) >> 10) << 3;
      uint g = ((*p & 0b0000001111100000) >>  5) << 3;
      uint b = ((*p & 0b0000000000011111) >>  0) << 3;*/
      p++;

/*      if(r > 200 && g > 200 && b > 200) {
        r = hue_map[x][0];
        g = hue_map[x][1];
        b = hue_map[x][2];
      }*/
      galactic_unicorn.set_pixel(x, y, 0, 0, 255);
    }


    sleep_ms(10);
  }



  printf("done\n");

  return 0;
}
