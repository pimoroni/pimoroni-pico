#include <math.h>
#include <vector>

#include "picographics_st7789.hpp"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

PicoGraphicsST7789 display(WIDTH, HEIGHT, ROTATE_0, false, nullptr, get_spi_pins(BG_SPI_FRONT));

int main() {
  display.set_backlight(255);

  // Delete the default palette and allow us to create up to 256 of our own RGB565 colours
  display.set_palette_mode(PicoGraphicsST7789::PaletteModeUSER);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    Pen      pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.r = (rand() % 10) + 3;
    shape.x = rand() % (display.bounds.w - (shape.r * 2));
    shape.y = rand() % (display.bounds.h - (shape.r * 2));
    shape.x += shape.r;
    shape.y += shape.r;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = display.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Pen BG = display.create_pen(120, 40, 60);
  Pen WHITE = display.create_pen(255, 255, 255);

  while(true) {
    display.set_pen(BG);
    display.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < shape.r) shape.dx *= -1;
      if(shape.x >= display.bounds.w - shape.r) shape.dx *= -1;
      if(shape.y < shape.r) shape.dy *= -1;
      if(shape.y >= display.bounds.h - shape.r) shape.dy *= -1;

      display.set_pen(shape.pen);
      display.circle(Point(shape.x, shape.y), shape.r);
    }

    display.set_pen(WHITE);
    display.text("Hello World", Point(0, 0), 240);

    // update screen
    display.update();
  }

    return 0;
}
