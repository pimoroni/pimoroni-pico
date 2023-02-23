#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "libraries/pico_display_2/pico_display_2.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

ST7789 st7789(320, 240, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

RGBLED led(PicoDisplay2::LED_R, PicoDisplay2::LED_G, PicoDisplay2::LED_B);

Button button_a(PicoDisplay2::A);
Button button_b(PicoDisplay2::B);
Button button_x(PicoDisplay2::X);
Button button_y(PicoDisplay2::Y);

int main() {
  st7789.set_backlight(255);

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

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  while(true) {
    if(button_a.raw()) text_location.x -= 1;
    if(button_b.raw()) text_location.x += 1;

    if(button_x.raw()) text_location.y -= 1;
    if(button_y.raw()) text_location.y += 1;
  
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

    // Since HSV takes a float from 0.0 to 1.0 indicating hue,
    // then we can divide millis by the number of milliseconds
    // we want a full colour cycle to take. 5000 = 5 sec
    RGB p = RGB::from_hsv((float)millis() / 5000.0f, 1.0f, 0.5f + sinf(millis() / 100.0f / 3.14159f) * 0.5f);

    led.set_rgb(p.r, p.g, p.b);


    graphics.set_pen(WHITE);
    graphics.text("Hello World", text_location, 320);

    // update screen
    st7789.update(&graphics);
  }

    return 0;
}
