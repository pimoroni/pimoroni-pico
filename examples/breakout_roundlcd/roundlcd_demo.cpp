#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "time.h"

// Place a 1.3 Round SPI LCD in the *front* slot of breakout garden.

using namespace pimoroni;


const int WIDTH = 240;
const int HEIGHT = 240;

ST7789 st7789(WIDTH, HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

constexpr float RADIUS = WIDTH / 2;

Pen from_hsv(float h, float s, float v) {
  uint8_t r = 0, g = 0, b = 0;

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

  return graphics.create_pen(r, g, b);
}

int main() {
    st7789.set_backlight(255);

    uint32_t steps = 70;
    float angle_step = 0.5f;

    Pen BLACK = graphics.create_pen(0, 0, 0);
    Pen WHITE = graphics.create_pen(255, 255, 255);

    while(1) {
        absolute_time_t at = get_absolute_time();
        uint64_t t = to_us_since_boot(at) / 100000;
        float angle = (t % 360) * M_PI / 180.0f;

        graphics.set_pen(BLACK);
        graphics.clear();
        graphics.set_pen(WHITE);

        for(auto step = 0u; step < steps; step++) {
          auto distance = RADIUS / steps * step;
          distance += step * 0.2f;

          angle += angle_step;

          unsigned int x = RADIUS + (distance * cosf(angle));
          unsigned int y = RADIUS + (distance * sinf(angle));

          auto l = 2.0 + (((sin((t / 2.0f) + angle) + 1) / 2.0f) * 8.0f);

          auto p = from_hsv((t / 10.0f) + distance / 120.0f, 1.0, 1.0);

          graphics.set_pen(p);
          graphics.circle(Point(x, y), l);
        }

        st7789.update(&graphics);
        sleep_ms(10);
    }
}