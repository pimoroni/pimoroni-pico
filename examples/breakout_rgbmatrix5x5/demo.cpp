#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "breakout_rgbmatrix5x5.hpp"

using namespace pimoroni;

BreakoutRGBMatrix5x5 rgbmatrix5x5;

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// This wonderful lookup table maps the LEDs on RGB Matrix 5x5
// from their 3x5x5 (remember, they're RGB) configuration to
// their specific location in the 144 pixel buffer.
const RGB lookup_table[] = {
  (118, 69, 85),
  (117, 68, 101),
  (116, 84, 100),
  (115, 83, 99),
  (114, 82, 98),
  (113, 81, 97),
  (112, 80, 96),
  (134, 21, 37),
  (133, 20, 36),
  (132, 19, 35),
  (131, 18, 34),
  (130, 17, 50),
  (129, 33, 49),
  (128, 32, 48),
  (127, 47, 63),
  (121, 41, 57),
  (122, 25, 58),
  (123, 26, 42),
  (124, 27, 43),
  (125, 28, 44),
  (126, 29, 45),
  (15, 95, 111),
  (8, 89, 105),
  (9, 90, 106),
  (10, 91, 107),
  (11, 92, 108),
  (12, 76, 109),
  (13, 77, 93),
};

RGB lookup_pixel(uint8_t index) {
  return lookup_table[index];
}

void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  if(x == 1 || x == 3) {
    y = 4 - y;
  }
  uint8_t index = y + (x * 5);
  RGB rgb = lookup_pixel(index);
  rgbmatrix5x5.set(rgb.r, r);
  rgbmatrix5x5.set(rgb.g, g);
  rgbmatrix5x5.set(rgb.b, b);
}

int main() {
  rgbmatrix5x5.init();
  rgbmatrix5x5.enable({
    0b00000000, 0b10111111,
    0b00111110, 0b00111110,
    0b00111111, 0b10111110,
    0b00000111, 0b10000110,
    0b00110000, 0b00110000,
    0b00111111, 0b10111110,
    0b00111111, 0b10111110,
    0b01111111, 0b11111110,
    0b01111111, 0b00000000
  }, 0);

  uint8_t col = 0;
  while(1) {
    for(auto x = 0u; x < 5; x++) {
      for(auto y = 0u; y < 5; y++) {
        set_pixel(x, y, col, col, col);
      }
    }
    rgbmatrix5x5.update(0);
    sleep_ms(10);
    col++;
  }
  return 0;
}
