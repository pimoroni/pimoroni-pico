// Example for testing Async DMA and Partial Updates
// 
// bottons at bottom of display show current settings, tap to change.
// From left to right:
// Button 1: Change rotation
// Button 2: Cycles around pen (display) format - PenRGB565, PenRGB332, Pen* and Pen4
// Button 3: Turns Async DMA on and off
// Button 4: Cycles around Partial Region updates: None, Half, Bounce and Full
// Button 5: Run touchscreen calibration, this will display to the log the c line needed to calibrate your screen.
//
// Timings are logged to the screen and also the console
// I = The pen format, Async status and region.
// C = The time taken in ms for calculations
// D = The remaining free DMA time in ms, so the amount of free time you have for more calculations.
// R = The time to render the frame in ms.
// U = The time blocked in updating the display in ms.
// T = The total time in ms.
//
// Async Updates
// -------------
// Async updates will only have an effect if you are using the native pen format 565
// If this is turned on then when you call Update() it will return immediately after setting up the DMA
// At this point you can use the processor as usual apart from you must not send anything to the spi
// or to the display buffer. In this example we are doing the calculations for the bouncing pixels here.
// Async updates also work for region updates using 565.
//
// So when this example first starts dma is off, in a release build here we see these timings:
//  c=9.06, d=0.00, r=10.43, u=20.89, t=40.38
// If we turn Async DMA on with the X button we see:
//  c=9.05, d=10.60, r=10.43, u=0.00, t=30.09
//
// So we can see the time we take to do the calculations (C) stays the same.
// The time to render stays (R) the same
// The time to update the display has dropped from 20.89ms to 0.00ms though
//  What is happening is that the display is now being updated via DMA
//  So now we are doing our cacluations whilst this is happening.
//  so now with Async on we can use that previously blocked time for our calculations.
// We can see the total time drop from 40.38ms to 30.09ms, a nice little speedup. 
// And there is another 10.60ms available if we had more calculations
//
// Partial Updates
// ===============
//
// There may be cases where you don't want to update the whole display.
// If you know you have only updated a small part of the framebuffer then you can use Partial Updates to only send that part to the display.
//
// This example allows you to test partial_update()
// Using Button Y you can cycle around different modes:
// None:   Partial updates turned off.
// Half:   The left half of the display is updated.
// Bounce: A quarter of the display is updated with this region bouncing around. For testing clipping.
// Full:	 The full display is updated as a partial area, Currently this can speed things up for some pens
//         For example P4 drops from around 65.2ms to 42.26ms when using partial updates for the full screen.

#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/ili9341/ili9341.hpp"
#include "drivers/xpt2046/xpt2046.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"

#include "calibrate_screen.hpp"
#include "elapsed_us.hpp"

using namespace pimoroni;


#define NUM_PIXELS (3000)
#define BUTTON_HEIGHT (32)
#define ILI941_WIDTH (240)
#define ILI941_HEIGHT (320)
#define XPT2046_WIDTH (240)
#define XPT2046_HEIGHT (320)
#define XPT2046_ROTATION_OFFSET (0)

enum UsePen{up565, up332, up8, up4, upCount};
enum UseRegion {urNone, urHalf, urBounce, urFull, urCount};

static const char* pen_strings[] = {"565", "332", "P8", "P4"};
static const char* region_strings[] = {"None", "Half", "Bounce", "Full"};

SPIPins touch_spi = {spi1, 14, 10, 11, 8, PIN_UNUSED, PIN_UNUSED};
XPT2046* xpt2046 = nullptr;
ILI9341* ili9341 = nullptr;
PicoGraphics* graphics = nullptr;

// default to async dma off
bool use_async_dma = false;

// default to landscape pins on left rotation
Rotation rotation = ROTATE_270;

// pens
Pen black_pen, white_pen;

// default to 565
UsePen use_pen = up565;

// default to no region
UseRegion use_region = urNone;
Rect region;
Point bounce = {0, 0};
Point bounce_inc = {1, 1};

// buffer for screen messages
char  log_buffer[64];

// allocate our own framebuffer to share between pens/formats
// this stops any memory fragmentation
uint8_t framebuffer[ILI941_WIDTH*ILI941_HEIGHT*2];

int main() {
	stdio_init_all();

  struct pt {
    float      x;
    float      y;
    float     dx;
    float     dy;
    uint16_t use_pen;
  };

  
	std::vector<pt> pixels(NUM_PIXELS);
	uint update_time = 0;
	uint calc_time = 0;
	uint dma_time = 0;
	uint render_time = 0;
	uint total_time = 0;
		
	bool last_touch_state = false;
	
  while(true) {
		ElapsedUs total_timer;
		ElapsedUs timer;

		// check buttons
		enum Button { btNone, btRotate, btPen, btAsync, btRegion, btCalibrate };
		Button btn = btNone;

		if(xpt2046)	{
			xpt2046->update();
		}

		bool touch_state = xpt2046->is_touched();
		if(touch_state != last_touch_state)
		{
			last_touch_state = touch_state;
			if(touch_state) {
				TouchPoint touch = xpt2046->get_touch();

				if(touch.y > graphics->bounds.h - BUTTON_HEIGHT) {
					btn = (Button)(1+(touch.x/(graphics->bounds.w/5)));
				}
			}
		}

		if(btn == btCalibrate) {
			Calibrate_Screen::run_calibration(ili9341, xpt2046, graphics);
		}

		// cycle around regions
		if(btn == btRegion){
			use_region = (UseRegion)((use_region+1) % urCount);
		}

		if(ili9341 == nullptr || btn == btRotate || btn == btPen || btn == btAsync) {
			// switch async DMA mode
			if(btn == btAsync) {
				use_async_dma = ! use_async_dma;
				delete ili9341;
				ili9341 = nullptr;
			}

			// cycle around rotations
			if(btn == btRotate) {
				rotation = (Rotation)((rotation + 90) % 360);
				delete ili9341;
				ili9341 = nullptr;
				delete xpt2046;
				xpt2046 = nullptr;
			}

			// cycle around pens (graphics mode)
			if(btn == btPen) {
				use_pen = (UsePen)((use_pen + 1) % upCount);
			}

			SPIPins spi_pins = get_spi_pins(BG_SPI_FRONT);
			if(ili9341 == nullptr) {
				ili9341  = new ILI9341(ILI941_WIDTH, ILI941_HEIGHT, rotation, false, spi_pins, 21, 62500000, use_async_dma);
				xpt2046 = new XPT2046(XPT2046_WIDTH, XPT2046_HEIGHT, (Rotation)((rotation+XPT2046_ROTATION_OFFSET)%360), touch_spi, 15);
			  ili9341->set_backlight(255);
			}

			delete graphics;
			switch(use_pen)
			{
				case up565 : 	graphics = new PicoGraphics_PenRGB565(ili9341->width, ili9341->height, framebuffer); break;
				case up332 : 	graphics = new PicoGraphics_PenRGB332(ili9341->width, ili9341->height, framebuffer); break;
				case up4   : 	graphics = new PicoGraphics_PenP4(ili9341->width, ili9341->height, framebuffer); break;
				case up8  : 	graphics = new PicoGraphics_PenP8(ili9341->width, ili9341->height, framebuffer); break;
				default: break;
			}


		  black_pen = graphics->create_pen(0, 0, 0);
			white_pen = graphics->create_pen(255, 255, 255);

			graphics->set_font("bitmap8");

			pixels.clear();
			for(int i = 0; i < NUM_PIXELS; i++) {
				pt pixel;
				pixel.x = rand() % graphics->bounds.w;
				pixel.y = rand() % graphics->bounds.h - BUTTON_HEIGHT;
				pixel.dx = float(rand() % 255) / 128.0f;
				pixel.dy = float(rand() % 255) / 128.0f;
				pixel.use_pen = graphics->create_pen(rand() % 255, rand() % 255, rand() % 255);
				pixels.push_back(pixel);
			}
		}

		switch (use_region)
		{
			case urFull   : region = Rect(0, 0, ili9341->width, ili9341->height); break;
			case urHalf   : region = Rect(0, 0, ili9341->width / 2, ili9341->height); break;
			case urBounce : region = Rect(bounce.x - (ili9341->width / 4), bounce.y - (ili9341->height / 4), ili9341->width / 2, ili9341->height / 2); break;
			default: break;
		}

		// update data
    for(auto &pixel : pixels) {
      pixel.x += pixel.dx;
      pixel.y += pixel.dy;
      if(pixel.x < 0) pixel.dx *= -1;
      if(pixel.x >= graphics->bounds.w) pixel.dx *= -1;
      if(pixel.y < 0) pixel.dy *= -1;
      if(pixel.y >= graphics->bounds.h - BUTTON_HEIGHT) pixel.dy *= -1;
    }

		bounce.x += bounce_inc.x;
		bounce.y += bounce_inc.y;
		if(bounce.x < 0) bounce_inc.x = 1;
		if(bounce.x >= graphics->bounds.w) bounce_inc.x = -1;
		if(bounce.y < 0) bounce_inc.y = 1;
		if(bounce.y >= graphics->bounds.h) bounce_inc.y = -1;
		
   
		calc_time = timer.elapsed();

		// if async wait for last update to finish before rendering
		if(use_async_dma) {
			ili9341->wait_for_update_to_finish();
		}

		dma_time = timer.elapsed();

		// render
    graphics->set_pen(black_pen);
    graphics->clear();

    for(auto &pixel : pixels) {
      graphics->set_pen(pixel.use_pen);
			graphics->pixel(Point(pixel.x, pixel.y));
    }

		graphics->set_pen(white_pen);
		graphics->rectangle_frame(graphics->bounds);

		uint spacing = 20;
		float scale = 2;
		int y = -(spacing/2);
		sprintf(log_buffer,"p=%s, %s, %s", pen_strings[use_pen], use_async_dma ? "A" : "S", region_strings[use_region]);
		printf("%s, ", log_buffer);

		sprintf(log_buffer,"c=%u.%.2u", calc_time/1000, (calc_time - ((calc_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"d=%u.%.2u", dma_time/1000, (dma_time - ((dma_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"r=%u.%.2u", render_time/1000, (render_time - ((render_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"u=%u.%.2u", update_time/1000, (update_time - ((update_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"t=%u.%.2u", total_time/1000, (total_time - ((total_time/1000)*1000)) / 10);
		printf("%s\n", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		if(use_region == urBounce)
		{
			graphics->set_pen(white_pen);
			graphics->line(Point(region.x+1, region.y+1), Point(region.x+1+region.w-3, region.y+1));
			graphics->line(Point(region.x+1+region.w-3, region.y+1), Point(region.x+1+region.w-3, region.y+1+region.h-3));
			graphics->line(Point(region.x+1+region.w-3, region.y+1+region.h-3), Point(region.x+1, region.y+1+region.h-3));
			graphics->line(Point(region.x+1, region.y+1+region.h-3), Point(region.x+1, region.y+1));
		}

		if (xpt2046->is_touched())
		{
			TouchPoint touch = xpt2046->get_touch();
			printf("Touch %ld, %ld, %ld\n", touch.x, touch.y, touch.z);
			graphics->set_pen(white_pen);
			graphics->circle(Point(touch.x, touch.y), 1+(touch.z/5));
		}

		// menu
		uint button_width  = graphics->bounds.w/5;
		uint button_height = BUTTON_HEIGHT;
		uint y1 = graphics->bounds.h - button_height;

		const char *strs[] = { "Rot", pen_strings[use_pen], use_async_dma ? "A" : "S", region_strings[use_region], "Cali" };

		for(uint b = 0; b < 5; b++) {
			Rect r(b * button_width, y1, button_width, button_height);
			graphics->rectangle_frame(r);
			int32_t text_width = graphics->measure_text(strs[b]);
			uint offset = (button_width - text_width)/2;
			graphics->text(strs[b], Point(r.x+offset, r.y+8), r.x+button_width, scale);

		}
		
		render_time = timer.elapsed();

		// now update the display
		if(use_region != urNone){
	    ili9341->partial_update(graphics, region);
		}
		else {
    	ili9341->update(graphics);
		}

		update_time = timer.elapsed();
		total_time  = total_timer.elapsed();
  }

  return 0;
}
