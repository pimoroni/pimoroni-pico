#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "hub75.hpp"
#include "okcolor.hpp"

using namespace pimoroni;

//If the display looks streaky or corrupted then uncomment one of the other initalisers

//Works with our 32x32 panels https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=35962488650 https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=19321740999
Hub75 hub75(32, 32, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(64, 32, nullptr, PANEL_GENERIC, false);

//and 64x32 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=42312764298
//Hub75 hub75(64, 32, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(128, 32, nullptr, PANEL_GENERIC, false);

//Works with our 64x64 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=3029531983882
//Hub75 hub75(64, 64, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(128, 64, nullptr, PANEL_GENERIC, false);

PicoGraphics_PenRGB888 graphics(hub75.width, hub75.height, nullptr);

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
  p.x += (hub75.width / 2) - (w / 2);
  p.y += (hub75.height / 2);
  graphics.text(t, Point(p.x, p.y), -1, s, a);
  //graphics.text(t, Point(p.x + 1, p.y), -1, s, a);
  //graphics.text(t, Point(p.x + 1, p.y + 1), -1, s, a);
  //graphics.text(t, Point(p.x, p.y + 1), -1, s, a);
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

  stdio_init_all();

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

  hub75.init();

/*
  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;
*/
  graphics.set_font("bitmap8");



  float i = 0;

  float hue_offset = 0.0f;

  bool animate = true;

  float stripe_width = 3.0f;
  float speed = 1.0f;
  float curve = 0.0f;

  while(true) {

    if(animate) {
      i += speed;
    }


    if(hub75.is_pressed(hub75.SWITCH_A)) {
      speed += 0.05f;
      speed = speed >= 10.0f ? 10.0f : speed;
      animate = true;
    }
    if(hub75.is_pressed(hub75.SWITCH_B)) {
      speed -= 0.05f;
      speed = speed <= 0.0f ? 0.0f : speed;
      animate = true;
    }

    for(int x = 0; x < 53; x++) {
      for(int y = 0; y < 11; y++) {
        int v = ((sin((x + y) / stripe_width + (sin((y * 3.1415927f * 2.0f) / 11.0f) * curve) + i / 15.0f) + 1.5f) / 2.5f) * 255.0f;

        uint8_t r = (hue_map[x][0] * v) / 256;
        uint8_t g = (hue_map[x][1] * v) / 256;
        uint8_t b = (hue_map[x][2] * v) / 256;

        graphics.set_pen(r, g, b);
        graphics.pixel(Point(x, y));
      }
    }
    hub75.update(&graphics);

    printf("%d\n", hub75.light());
    sleep_ms(20);
  }



  printf("done\n");

  return 0;
}
