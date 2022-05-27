#include <math.h>
#include <vector>

#include "breakout_colourlcd160x80.hpp"

using namespace pimoroni;

uint8_t buffer[BreakoutColourLCD160x80::WIDTH * BreakoutColourLCD160x80::HEIGHT];
BreakoutColourLCD160x80 lcd((void *)buffer);

int main() {
  lcd.init();
  lcd.set_backlight(255);

  // Delete the default palette and allow us to create up to 256 of our own RGB565 colours
  lcd.set_palette_mode(BreakoutColourLCD160x80::PaletteModeUSER);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    Pen      pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 1000; i++) {
    pt shape;
    shape.x = rand() % lcd.bounds.w;
    shape.y = rand() % lcd.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 128.0f;
    shape.dy = float(rand() % 255) / 128.0f;
    shape.pen = lcd.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  uint8_t bg = lcd.create_pen(120, 40, 60);

  while(true) {
    lcd.set_pen(bg);
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
