#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/interstate75/interstate75.hpp"

using namespace pimoroni;

RGBLED led_rgb(Interstate75::LED_R, Interstate75::LED_G, Interstate75::LED_B, Polarity::ACTIVE_LOW);
Button button_a(Interstate75::A);
Button button_b(Interstate75::B);



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
//Hub75 hub75(64, 64, nullptr, PANEL_GENERIC, true);
//or using 2 of these panels
//Hub75 hub75(128, 64, nullptr, PANEL_GENERIC, false);

PicoGraphics_PenRGB888 graphics(hub75.width, hub75.height, nullptr);

// Interrupt callback required function 
void __isr dma_complete() {
  hub75.dma_complete();
}

int main() {

  stdio_init_all();

  uint8_t hue_map[hub75.width][3];
  for(uint i = 0; i < hub75.width; i++) {
    RGB p = RGB::from_hsv(i / (float) hub75.width, 1.0f, 0.7f);
    hue_map[i][0] =  p.r;
    hue_map[i][1] =  p.g;
    hue_map[i][2] =  p.b;
  }

  hub75.start(dma_complete);
  graphics.set_font("bitmap8");

  float i = 0;
  bool animate = true;
  float stripe_width = 3.0f;
  float speed = 5.0f;
  float curve = 0.0f;
  float led_h = 0.0f;

  while(true) {

    if(animate) {
      i += speed;
    }

    if(button_a.raw()) {
      speed += 0.1f;
      speed = speed >= 10.0f ? 10.0f : speed;
      animate = true;
    }
    if(button_b.raw()) {
      speed -= 0.1f;
      speed = speed <= 0.0f ? 0.0f : speed;
      animate = true;
    }

    for(uint x = 0; x < hub75.width; x++) {
      for(uint y = 0; y < hub75.height; y++) {
        int v = ((sin((x + y) / stripe_width + (sin((y * 3.1415927f * 2.0f) / (float)hub75.width) * curve) + i / 15.0f) + 1.5f) / 2.5f) * 255.0f;

        uint8_t r = (hue_map[x][0] * v) / 256;
        uint8_t g = (hue_map[x][1] * v) / 256;
        uint8_t b = (hue_map[x][2] * v) / 256;

        graphics.set_pen(r, g, b);
        graphics.pixel(Point(x, y));
    }
    }
    hub75.update(&graphics);

    led_rgb.set_hsv(led_h, 1.0f, 1.0f);
    led_h += 0.01;


    //sleep_ms(5);
  }


}

