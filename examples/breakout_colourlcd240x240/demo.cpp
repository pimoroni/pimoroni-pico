#include <math.h>
#include <vector>

#include "generic_st7789.hpp"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

ST7789Generic display(WIDTH, HEIGHT, ROTATE_0, false, nullptr, get_spi_pins(BG_SPI_FRONT));

int main() {
  //lcd.configure_display(false);
  lcd.set_backlight(255);

  // Delete the default palette and allow us to create up to 256 of our own RGB565 colours
  lcd.set_palette_mode(ST7789Generic::PaletteModeUSER);

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
    shape.x = rand() % (lcd.bounds.w - (shape.r * 2));
    shape.y = rand() % (lcd.bounds.h - (shape.r * 2));
    shape.x += shape.r;
    shape.y += shape.r;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = lcd.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Pen BG = lcd.create_pen(120, 40, 60);
  Pen WHITE = lcd.create_pen(255, 255, 255);

  while(true) {
    lcd.set_pen(BG);
    lcd.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < shape.r) shape.dx *= -1;
      if(shape.x >= lcd.bounds.w - shape.r) shape.dx *= -1;
      if(shape.y < shape.r) shape.dy *= -1;
      if(shape.y >= lcd.bounds.h - shape.r) shape.dy *= -1;

      lcd.set_pen(shape.pen);
      lcd.circle(Point(shape.x, shape.y), shape.r);
    }

    lcd.set_pen(WHITE);
    lcd.text("Hello World", Point(0, 0), 240);

    // update screen
    lcd.update();
  }

    return 0;
}
