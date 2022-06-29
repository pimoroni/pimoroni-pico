#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"

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


int main() {

  uint8_t hue_map[53][3];
  for(int i = 0; i < 53; i++) {
    from_hsv(i / 53.0f, 1.0f, 1.0f, hue_map[i][0], hue_map[i][1], hue_map[i][2]);
  }

  star_t stars[100];
  for(int i = 0; i < 100; i++) {
    init_star(stars[i]);
    stars[i].a = i;
  }

gpio_set_function(28, GPIO_FUNC_SIO);
    gpio_set_dir(28, GPIO_OUT);

  for(int i = 0; i < 10; i++) {
    gpio_put(28, !gpio_get(28));
    sleep_ms(100);
  }
  sleep_ms(1000);

  gpio_put(28,true);

  galactic_unicorn.init();

/*
  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;
*/
  graphics.set_font("sans");



  uint i = 0;
  int v = 255;
  while(true) {
    i++;

    graphics.set_pen(0, 0, 0);
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_A)) {graphics.set_pen(255, 0, 0);}
    graphics.clear();

    
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {v = v == 0 ? 0 : v - 1;}


    for(int i = 0; i < 100; i++) {
      star_t &star = stars[i];
      step_star(star);

      uint b = star.brightness();
      graphics.set_pen(b, b, b);
      graphics.pixel(Point(star.x + (53 / 2), star.y + (11 / 2)));
    }

graphics.set_pen(255, 255, 255);
    float s = 1.0f;//0.65f + (sin(i / 25.0f) * 0.15f);
    float a = 1.0f;// (sin(i / 25.0f) * 100.0f);
    float x = (sin(i / 25.0f) * 40.0f) * s;
    float y = (cos(i / 15.0f) * 10.0f) * s;
    text("Galactic", Point(x, y), s, a);
    
    uint16_t *p = (uint16_t *)graphics.frame_buffer;
    for(size_t i = 0; i < 53 * 11; i++) {
      int x = i % 53;
      int y = i / 53;
      uint r = ((*p & 0b1111100000000000) >> 11) << 3;
      uint g = ((*p & 0b0000011111100000) >>  5) << 2;
      uint b = ((*p & 0b0000000000011111) >>  0) << 3;
      p++;

      if(r > 200 && g > 200 && b > 200) {
        r = hue_map[x][0];
        g = hue_map[x][1];
        b = hue_map[x][2];
      }
      galactic_unicorn.set_pixel(x, y, r, g, b);
    }


    sleep_ms(10);
  }



  printf("done\n");

  return 0;
}
