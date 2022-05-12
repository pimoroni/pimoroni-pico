#include <math.h>
#include <vector>

#include "generic_st7789.hpp"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

ST7789Generic lcd(WIDTH, HEIGHT, false, nullptr, BG_SPI_FRONT);

int main() {
  //lcd.configure_display(false);
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

  while(true) {
    lcd.set_pen(120, 40, 60);
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

    lcd.set_pen(255, 255, 255);
    lcd.text("Hello World", Point(0, 0), 240);

    // update screen
    lcd.update();
  }

    return 0;
}
