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

#define ILI941_WIDTH (240)
#define ILI941_HEIGHT (320)
// you might need to lower the baud rate
// try 156'250'000 then 312'500'000 if you are having issues
#define ILI941_BAUD_RATE (625'000'000)

// if you do not have a reset pin wired up change to
// #define RESET_PIN PIN_UNUSED
#define ILI941_RESET_PIN (21)

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
 
ILI9341 ili9341(ILI941_WIDTH, ILI941_HEIGHT, rotation, false, get_spi_pins(BG_SPI_FRONT), ILI941_RESET_PIN, ILI941_BAUD_RATE);
PicoGraphics_PenRGB332 graphics(ili9341.width, ili9341.height, nullptr);

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

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % graphics.bounds.w;
    shape.y = rand() % graphics.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);

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

    graphics.set_pen(BG);
    graphics.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if((shape.x - shape.r) < 0) {
        shape.dx *= -1;
        shape.x = shape.r;
      }
      if((shape.x + shape.r) >= graphics.bounds.w) {
        shape.dx *= -1;
        shape.x = graphics.bounds.w - shape.r;
      }
      if((shape.y - shape.r) < 0) {
        shape.dy *= -1;
        shape.y = shape.r;
      }
      if((shape.y + shape.r) >= graphics.bounds.h) {
        shape.dy *= -1;
        shape.y = graphics.bounds.h - shape.r;
      }

      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);
    }


    graphics.set_pen(WHITE);
    graphics.text(msg, text_location, 320);

    // update screen
    ili9341.update(&graphics);
  }

  return 0;
}
