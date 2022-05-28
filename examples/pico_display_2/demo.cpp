#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display_2.hpp"
#include "generic_st7789.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

ST7789Generic pico_display(240, 240, ROTATE_0);

RGBLED led(PicoDisplay2::LED_R, PicoDisplay2::LED_G, PicoDisplay2::LED_B);

Button button_a(PicoDisplay2::A);
Button button_b(PicoDisplay2::B);
Button button_x(PicoDisplay2::X);
Button button_y(PicoDisplay2::Y);

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

int main() {
  pico_display.set_backlight(255);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % pico_display.bounds.w;
    shape.y = rand() % pico_display.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = pico_display.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  Pen BG = pico_display.create_pen(120, 40, 60);
  Pen WHITE = pico_display.create_pen(255, 255, 255);

  while(true) {
    if(button_a.raw()) text_location.x -= 1;
    if(button_b.raw()) text_location.x += 1;

    if(button_x.raw()) text_location.y -= 1;
    if(button_y.raw()) text_location.y += 1;
  
    pico_display.set_pen(BG);
    pico_display.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if((shape.x - shape.r) < 0) {
        shape.dx *= -1;
        shape.x = shape.r;
      }
      if((shape.x + shape.r) >= pico_display.bounds.w) {
        shape.dx *= -1;
        shape.x = pico_display.bounds.w - shape.r;
      }
      if((shape.y - shape.r) < 0) {
        shape.dy *= -1;
        shape.y = shape.r;
      }
      if((shape.y + shape.r) >= pico_display.bounds.h) {
        shape.dy *= -1;
        shape.y = pico_display.bounds.h - shape.r;
      }

      pico_display.set_pen(shape.pen);
      pico_display.circle(Point(shape.x, shape.y), shape.r);

    }

    // Since HSV takes a float from 0.0 to 1.0 indicating hue,
    // then we can divide millis by the number of milliseconds
    // we want a full colour cycle to take. 5000 = 5 sec.
    uint8_t r = 0, g = 0, b = 0;
    from_hsv((float)millis() / 5000.0f, 1.0f, 0.5f + sinf(millis() / 100.0f / 3.14159f) * 0.5f, r, g, b);
    led.set_rgb(r, g, b);


    pico_display.set_pen(WHITE);
    pico_display.text("Hello World", text_location, 320);

    // update screen
    pico_display.update();
  }

    return 0;
}
