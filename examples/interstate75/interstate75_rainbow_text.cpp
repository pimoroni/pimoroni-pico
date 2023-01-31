#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/hub75/hub75.hpp"

using namespace pimoroni;


// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
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
  graphics.text(t, Point(p.x + 1, p.y), -1, s, a);
  graphics.text(t, Point(p.x + 1, p.y + 1), -1, s, a);
  graphics.text(t, Point(p.x, p.y + 1), -1, s, a);
}

// Interrupt callback required function 
void __isr dma_complete() {
  hub75.dma_complete();
}

int main() {
  //Start hub75 driver
  hub75.start(dma_complete);

  uint8_t hue_map[hub75.width][3];
  for(int i = 0; i < (int)hub75.width; i++) {
    from_hsv(i / (float)hub75.width, 1.0f, 1.0f, hue_map[i][0], hue_map[i][1], hue_map[i][2]);
  }

  graphics.set_font("sans");
  uint i = 0;

  while(true) {
    i++;
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    float s = 0.8f;//0.65f + (sin(i / 25.0f) * 0.15f);
    float a = 2.0f;// (sin(i / 25.0f) * 100.0f);

    float x = (sin((i) / 50.0f) * 190.0f);
    float y = (cos((i) / 30.0f) * 10.0f - 10.0f);
    graphics.set_pen(255, 255, 255);
    text("Interstate 75 HUB75 display driver", Point(x, y), s, a);

    uint8_t *p = (uint8_t *)graphics.frame_buffer;
    for(size_t i = 0; i < hub75.width * hub75.height; i++) {
      int x = i % hub75.width;
      int y = i / hub75.width;
      uint r = *p++;
      uint g = *p++;
      uint b = *p++;
      p++;

      if(r > 0) {
        r = hue_map[x][0];
        g = hue_map[x][1];
        b = hue_map[x][2]; 
      }

      graphics.set_pen(r, g, b);
      graphics.pixel(Point(x, y));
    }
    hub75.update(&graphics);
  }

  printf("done\n");

  return 0;
}
