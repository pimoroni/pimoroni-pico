#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>


#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/hub75/hub75.hpp"

using namespace pimoroni;

const uint8_t WIDTH = 32;
const uint8_t HEIGHT = 32;
const uint8_t QTY_BALLS = 15;

//If the display looks streaky or corrupted then uncomment one of the other initalisers

//Works with our 32x32 small pixel panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=35962488650
//Hub75 hub75(WIDTH, HEIGHT, PANEL_GENERIC, true);

//Works with our 32x32 large pixel panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=19321740999
//and 64x32 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=42312764298
Hub75 hub75(WIDTH, HEIGHT, nullptr, PANEL_FM6126A, false, 2, 1);

//Works with our 64x64 panel https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=3029531983882
//Hub75 hub75(WIDTH, HEIGHT, PANEL_FM6126A, false);

//Hub75 hub75(WIDTH, HEIGHT, PANEL_FM6126A, true);

//PicoGraphics_PenRGB888M graphics(hub75.width, hub75.height, nullptr, 1, 1);
PicoGraphics_PenRGB888 graphics(hub75.width * 2, hub75.height * 1, nullptr);
// Callback for the dma interrupt (required)
void __isr dma_complete() {
    hub75.dma_complete();
}

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
  hub75.start(dma_complete);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    Pen pen;
  };

  std::vector<pt> shapes;
  for(uint8_t i = 0; i < QTY_BALLS; i++) {
    pt shape;
    shape.x = rand() % graphics.bounds.w;
    shape.y = rand() % graphics.bounds.h;
    shape.r = (rand() % 5) + 2;
    shape.dx = float(rand() % 255) / 128.0f;
    shape.dy = float(rand() % 255) / 128.0f;
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(200, 200, 200);

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
    graphics.text("Hello World", text_location, false, 1.0f);

    // update screen
    hub75.update(&graphics);
    sleep_ms(1000/10);
  }

    return 0;
}