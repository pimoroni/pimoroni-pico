#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

#include "font6_data.hpp"
#include "font8_data.hpp"
#include "msa301.hpp"

using namespace pimoroni;

extern unsigned char _binary_fox_tga_start[];

ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

MSA301 msa301;

int main() {
  graphics.set_font(&font8);
  msa301.init();

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  uint32_t i = 0;
  while(true) {
    graphics.set_pen(BG);
    graphics.clear();

    graphics.set_pen(WHITE);
    graphics.set_font(&font6);
    graphics.text("6x6: The quick, brown fox jumps over the lazy dog! UPPER. lower.", Point(10, 10), 220);
    graphics.text("0123456789 !$%^&*()", Point(10, 70), 220);
    graphics.set_font(&font8);
    graphics.text("6x8: The quick, brown fox jumps over the lazy dog! UPPER. lower.", Point(10, 120), 220);
    graphics.text("0123456789 !$%^&*()", Point(10, 180), 220);

    st7789.update(&graphics);

    i++;
  }

  return 0;
}
