#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "st7789.hpp"
#include "pico_graphics.hpp"

#define CS 17
#define DC 16
#define SCK 18
#define MOSI 19
#define MISO -1

using namespace pimoroni;

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
        screen.init(true, true);
        //screen.flip();
    };
    void update() {
        screen.update();
    }
};


uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay display(buffer);

int main() {
    display.init();

    while(1) {
        display.set_pen(0, 0, 0);
        display.clear();
        display.set_pen(255, 0, 0);
        display.circle(Point(60, 120), 60);
        display.set_pen(255, 255, 255);
        display.text("Hello", Point(10, 10), 220, 4);
        display.update();
    }
}