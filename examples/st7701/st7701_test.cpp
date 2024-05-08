#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "drivers/st7701/st7701.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

static const uint BACKLIGHT = 2;

static const int WIDTH = 480;
static const int HEIGHT = 480;
static const uint LCD_CLK = 29;
static const uint LCD_CS = 28;
static const uint LCD_DAT = 27;
static const uint LCD_DC = PIN_UNUSED;
static const uint LCD_D0 = 1;

ST7701 st7701(
  WIDTH,
  HEIGHT,
  ROTATE_0,
  SPIPins{
    spi0,
    LCD_CS,
    LCD_CLK,
    LCD_DAT,
    PIN_UNUSED, // MISO
    LCD_DC,
    BACKLIGHT
  },
  LCD_D0
);

PicoGraphics_PenRGB332 graphics(st7701.width, st7701.height, nullptr);

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

int main() {
  st7701.set_backlight(255);

  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen BG = graphics.create_pen(120, 40, 60);

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
    shape.x = rand() % graphics.bounds.w;
    shape.y = rand() % graphics.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  while(true) {
    
    graphics.set_pen(BG);
    graphics.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if((shape.x - shape.r) < 0) {
        shape.dx *= -1;
        shape.x = shape.r;
      }
      if((shape.x + shape.r) >= graphics.bounds.w) {
        shape.dx *= -1;
        shape.x = graphics.bounds.w - shape.r;
      }
      if((shape.y - shape.r) < 0) {
        shape.dy *= -1;
        shape.y = shape.r;
      }
      if((shape.y + shape.r) >= graphics.bounds.h) {
        shape.dy *= -1;
        shape.y = graphics.bounds.h - shape.r;
      }

      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);

    }


    graphics.set_pen(WHITE);
    graphics.text("Hello World", text_location, 320);

    // update screen
    st7701.update(&graphics);
  }

    return 0;
}
