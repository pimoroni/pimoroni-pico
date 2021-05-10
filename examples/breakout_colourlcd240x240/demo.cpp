#include <math.h>
#include <vector>

#include "breakout_colourlcd240x240.hpp"

using namespace pimoroni;

uint16_t buffer[BreakoutColourLCD240x240::WIDTH * BreakoutColourLCD240x240::HEIGHT];
BreakoutColourLCD240x240 lcd(buffer);

int main() {
  lcd.init();
  lcd.set_backlight(255);

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
    shape.x = rand() % lcd.bounds.w;
    shape.y = rand() % lcd.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = lcd.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  while(true) {
    lcd.set_pen(120, 40, 60);
    lcd.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= lcd.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= lcd.bounds.h) shape.dy *= -1;

      lcd.set_pen(shape.pen);
      lcd.circle(Point(shape.x, shape.y), shape.r);
    }

    // update screen
    lcd.update();
  }

    return 0;
}
