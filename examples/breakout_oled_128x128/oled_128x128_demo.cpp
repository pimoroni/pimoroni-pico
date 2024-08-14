#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/sh1107/sh1107.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "time.h"

using namespace pimoroni;


const int WIDTH = 128;
const int HEIGHT = 128;

I2C i2c(4, 5);

SH1107 sh1107(WIDTH, HEIGHT, i2c);
PicoGraphics_Pen1Bit graphics(sh1107.width, sh1107.height, nullptr);

int main() {


  uint i = 0;
  while(true) {
    // for(int y = 0; y < 128; y++) {
    //   for(int x = 0; x < 128; x++) {
    //     graphics.set_pen(((x + y + i) & 0b1111) > 8 ? 1 : 0);
    //     graphics.set_pixel({x, y});
    //   }
    // }

    graphics.set_pen(15);
    graphics.clear();

    float s = (sin(i / 10.0f) * 1.0f) + 1.5f;
    float a = (cos(i / 10.0f) * 10.0f);

    graphics.set_font("gothic");

    graphics.set_pen(0);
    int w = graphics.measure_text("PIRATES!", s);
    graphics.text("PIRATES!", {64 - w / 2, 60}, 0, s, a);

    sh1107.update(&graphics);

    //sleep_ms(10);

    i++;
  }
  
}