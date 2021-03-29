#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "st7789.hpp"
#include "pico_graphics.hpp"
#include "time.h"

// Place a 1.3 Round SPI LCD in the *front* slot of breakout garden.

#define CS 17
#define DC 16
#define SCK 18
#define MOSI 19
#define MISO -1

// "true" to init the screen in "round" mode
#define ROUND true

using namespace pimoroni;

// Create a PicoDisplay to bind PicoGraphics with our ST7789 display
class PicoDisplay : public PicoGraphics {
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 240;

    uint16_t *__fb;
  private:
    ST7789 screen;

  public:
    PicoDisplay(uint16_t *buf) : PicoGraphics(WIDTH, HEIGHT, buf),
    screen(WIDTH, HEIGHT, buf,
            spi0,
            CS, DC, SCK, MOSI, MISO) {
        __fb = buf;
    }

    void init() {
      screen.init(true, ROUND);
    };
    void update() {
      screen.update();
    }
};


uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay display(buffer);

constexpr float RADIUS = PicoDisplay::WIDTH / 2;

Pen from_hsv(float h, float s, float v) {
  uint8_t r, g, b;

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

  return display.create_pen(r, g, b);
}

int main() {
    display.init();

    uint32_t steps = 70;
    float angle_step = 0.5f;

    while(1) {
        absolute_time_t at = get_absolute_time();
        uint64_t t = at._private_us_since_boot / 100000;
        float angle = (t % 360) * M_PI / 180.0f;

        display.set_pen(0, 0, 0);
        display.clear();
        display.set_pen(255, 255, 255);

        for(auto step = 0u; step < steps; step++) {
          auto distance = RADIUS / steps * step;
          distance += step * 0.2f;

          angle += angle_step;

          unsigned int x = RADIUS + (distance * cosf(angle));
          unsigned int y = RADIUS + (distance * sinf(angle));

          auto l = 2.0 + (((sin((t / 2.0f) + angle) + 1) / 2.0f) * 8.0f);

          auto p = from_hsv((t / 10.0f) + distance / 120.0f, 1.0, 1.0);

          display.set_pen(p);
          display.circle(Point(x, y), l);
        }

        display.update();
    }
}