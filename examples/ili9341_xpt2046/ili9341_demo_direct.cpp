#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>


#include "drivers/ili9341/ili9341.hpp"
#include "drivers/xpt2046/xpt2046.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

const char* msg = "Hello ILI9341";

Rotation rotation = ROTATE_270;

#define ILI9341_WIDTH (240)
#define ILI9341_HEIGHT (320)
// you might need to lower the baud rate
// try 156'250'000 then 312'500'000 if you are having issues
#define ILI9341_BAUD_RATE (625'000'000)

// if you do not have a reset pin wired up change to
// #define RESET_PIN PIN_UNUSED
#define ILI9341_RESET_PIN (21)

// set to 1 if you have a XPT2046 touchscreen
#define USE_TOUCHSCREEN 1

#if USE_TOUCHSCREEN
  #define XPT2046_WIDTH (240)
  #define XPT2046_HEIGHT (320)
  #define XPT2046_ROTATION_OFFSET (0)
  #define XPT2046_CS (14)
  #define XPT2046_SCK (10)
  #define XPT2046_MOSI (11)
  #define XPT2046_MISO (8)
  #define XPT2046_IRQ (15)

  SPIPins touch_spi = {spi1, XPT2046_CS, XPT2046_SCK, XPT2046_MOSI, XPT2046_MISO, PIN_UNUSED, PIN_UNUSED};
  XPT2046	xpt2046(XPT2046_WIDTH, XPT2046_HEIGHT, (Rotation)((rotation+XPT2046_ROTATION_OFFSET)%360), touch_spi, XPT2046_IRQ);
#endif
 
ILI9341 ili9341(ILI9341_WIDTH, ILI9341_HEIGHT, rotation, false, get_spi_pins(BG_SPI_FRONT), ILI9341_RESET_PIN, ILI9341_BAUD_RATE);
PicoGraphics_PenRGB565_direct graphics(ili9341.width, ili9341.height, ili9341);

int main() {
  ili9341.set_backlight(255);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  Point text_location(0, 0);

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  graphics.set_pen(BG);
  graphics.clear();

  while(true) {  
    #if USE_TOUCHSCREEN
      xpt2046.update();
      if(xpt2046.is_touched()) {
        text_location = xpt2046.get_point();

        int32_t text_width = graphics.measure_text(msg);

        text_location.x -= text_width/2;
        text_location.y -= 8;
      }
    #endif

    for(uint c =0; c < 100; c++) {
      graphics.set_pen(rand() % 255, rand() % 255, rand() % 255);
      graphics.circle(Point(rand() % graphics.bounds.w, rand() % graphics.bounds.h), (rand() % 10) + 3);
    }

    graphics.set_pen(WHITE);
    graphics.text(msg, text_location, 320);
  }

  return 0;
}
