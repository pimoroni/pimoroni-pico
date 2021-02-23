#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"
#include "font6_data.hpp"
#include "font8_data.hpp"
#include "msa301.hpp"

using namespace pimoroni;

extern unsigned char _binary_fox_tga_start[];

uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);
MSA301 msa301;

int main() {
  pico_explorer.init();
  pico_explorer.set_font(&font8);
  msa301.init();

  uint32_t i = 0;
  while(true) {
    pico_explorer.set_pen(120, 40, 60);
    pico_explorer.clear();

    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.set_font(&font6);
    pico_explorer.text("6x6: The quick, brown fox jumps over the lazy dog! UPPER. lower.", Point(10, 10), 220);
    pico_explorer.text("0123456789 !$%^&*()", Point(10, 70), 220);
    pico_explorer.set_font(&font8);
    pico_explorer.text("6x8: The quick, brown fox jumps over the lazy dog! UPPER. lower.", Point(10, 120), 220);
    pico_explorer.text("0123456789 !$%^&*()", Point(10, 180), 220);

    pico_explorer.update();

    i++;
  }

  return 0;
}
