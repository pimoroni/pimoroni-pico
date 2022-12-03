// Example for testing Async DMA and Partial Updates
// 
// Button A: Cycles arround screen rotations
// Button B: Cycles around pen (display) format - PenRGB565, PenRGB332, Pen* and Pen4
// Button C: Turns Async DMA on and off
// Button D: Cycles around Partial Region updates: None, Half, Bounce and Full
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
//  C=8.92, D=0.00, R=7.67, U=9.86, T=26.46
// If we turn Async DMA on with the X button we see:
//  C=8.92, D=0.92, R=7.65, U=0.02, T=17.52
//
// So we can see the time we take to do the calculations (C) stays the same.
// The time to render stays (R) the same
// The time to update the display has dropped from 9.86ms to 0.02ms though
//  What is happening is that the display is now being updated via DMA
//  So now we are doing our cacluations whilst this is happening.
//  D is now set to .92ms, this is the time remaining till the display is ready (not using the dma)
//  if we add C, D and U we get 9.86ms which is the time taken when without async we were blocked updateing the display. 
//  so now with Async on we can use that previously blocked time for our calculations.
// We can see the total time drop from 26.46ms to 17.52ms, a nice little speedup.
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
//         For example P4 drops from around 34.3ms to 26.1ms when using partial updates for the full screen.

#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

class ElapsedUs
{
public:
	ElapsedUs()
	{
		last_time = time_us_64();
	}

	uint64_t elapsed(void)
	{
		uint64_t time_now = time_us_64();
		uint64_t elapsed = time_now - last_time;
		last_time = time_now;
		return elapsed;
	}

private:
	uint64_t last_time;
};

using namespace pimoroni;


Button button_a(PicoDisplay::A);
Button button_b(PicoDisplay::B);
Button button_x(PicoDisplay::X);
Button button_y(PicoDisplay::Y);

enum UsePen{up565, up332, up8, up4, upCount};
enum UseRegion {urNone, urHalf, urBounce, urFull, urCount};

static const char* pen_strings[] = {"565", "332", "P8", "P4"};
static const char* region_strings[] = {"None", "Half", "Bounce", "Full"};


int main() {
	stdio_init_all();

	// default to async dma off
	bool use_async_dma = false;

	// default to no rotation
	Rotation rotation = ROTATE_0;

	// default to 565
	UsePen use_pen = up565;

	// default to no region
	UseRegion use_region = urNone;
	Rect region;
	Point bounce = {0, 0};
	Point bounce_inc = {1, 1};


	ST7789* st7789 = nullptr;
	PicoGraphics* graphics = nullptr;

	char  log_buffer[64];

	// turn the led off
	RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);
	led.set_rgb(0, 0, 0);


  struct pt {
    float      x;
    float      y;
    float     dx;
    float     dy;
    uint16_t use_pen;
  };

  
	Pen black_pen, white_pen;

	std::vector<pt> pixels;
	uint update_time = 0;
	uint calc_time = 0;
	uint dma_time = 0;
	uint render_time = 0;
	uint total_time = 0;
		
	
  while(true) {
		ElapsedUs total_timer;
		ElapsedUs timer;

		bool change_rotation = button_a.read();
		bool change_pen = button_b.read();
		bool change_async = button_x.read();
		bool change_region = button_y.read();

		// cycle arounf regions
		if(change_region){
			use_region = (UseRegion)((use_region+1) % urCount);
		}

		if(st7789 == nullptr || change_rotation || change_pen || change_async) {
			// switch async DMA mode
			if(change_async) {
				use_async_dma = ! use_async_dma;
				delete st7789;
				st7789 = nullptr;
			}

			// cycle around rotations
			if(change_rotation) {
				rotation = (Rotation)((rotation + 90) % 360);
				delete st7789;
				st7789 = nullptr;
			}

			// cycle around pens (graphics mode)
			if(change_pen) {
				use_pen = (UsePen)((use_pen + 1) % upCount);
			}

			if(st7789 == nullptr)
				st7789 = new ST7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, rotation, false, get_spi_pins(BG_SPI_FRONT), use_async_dma);

			delete graphics;
			switch(use_pen)
			{
				case up565 : 	graphics = new PicoGraphics_PenRGB565(st7789->width, st7789->height, nullptr); break;
				case up332 : 	graphics = new PicoGraphics_PenRGB332(st7789->width, st7789->height, nullptr); break;
				case up8  : 	graphics = new PicoGraphics_PenP8(st7789->width, st7789->height, nullptr); break;
				case up4   : 	graphics = new PicoGraphics_PenP4(st7789->width, st7789->height, nullptr); break;
				default: break;
			}

		  st7789->set_backlight(150);
		  black_pen = graphics->create_pen(0, 0, 0);
			white_pen = graphics->create_pen(255, 255, 255);

			graphics->set_font("bitmap8");

			pixels.clear();
			for(int i = 0; i < 3000; i++) {
				pt pixel;
				pixel.x = rand() % graphics->bounds.w;
				pixel.y = rand() % graphics->bounds.h;
				pixel.dx = float(rand() % 255) / 128.0f;
				pixel.dy = float(rand() % 255) / 128.0f;
				pixel.use_pen = graphics->create_pen(rand() % 255, rand() % 255, rand() % 255);
				pixels.push_back(pixel);
			}
		}

		switch (use_region)
		{
			case urFull   : region = Rect(0, 0, st7789->width, st7789->height); break;
			case urHalf   : region = Rect(0, 0, st7789->width / 2, st7789->height); break;
			case urBounce : region = Rect(bounce.x - (st7789->width / 4), bounce.y - (st7789->height / 4), st7789->width / 2, st7789->height / 2); break;
			default: break;
		}

		// update data
    for(auto &pixel : pixels) {
      pixel.x += pixel.dx;
      pixel.y += pixel.dy;
      if(pixel.x < 0) pixel.dx *= -1;
      if(pixel.x >= graphics->bounds.w) pixel.dx *= -1;
      if(pixel.y < 0) pixel.dy *= -1;
      if(pixel.y >= graphics->bounds.h) pixel.dy *= -1;
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
			st7789->wait_for_update_to_finish();
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

		graphics->line(Point(0,0), Point(graphics->bounds.w-1, 0));
		graphics->line(Point(0,0), Point(0, graphics->bounds.h-1));

		graphics->line(Point(graphics->bounds.w-1,0), Point(graphics->bounds.w-1, graphics->bounds.h-1));
		graphics->line(Point(0,graphics->bounds.h-1), Point(graphics->bounds.w-1, graphics->bounds.h-1));

		graphics->set_pen(white_pen);

		uint spacing = 20;
		float scale = 2;
		int y = -(spacing/2);
		sprintf(log_buffer,"I=%s, %s, %s", pen_strings[use_pen], use_async_dma ? "A" : "S", region_strings[use_region]);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"C=%u.%.2u", calc_time/1000, (calc_time - ((calc_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"D=%u.%.2u", dma_time/1000, (dma_time - ((dma_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"R=%u.%.2u", render_time/1000, (render_time - ((render_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"U=%u.%.2u", update_time/1000, (update_time - ((update_time/1000)*1000)) / 10);
		printf("%s, ", log_buffer);
		graphics->text(std::string(log_buffer), Point(10, y+=spacing), graphics->bounds.w, scale);

		sprintf(log_buffer,"T=%u.%.2u", total_time/1000, (total_time - ((total_time/1000)*1000)) / 10);
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

		render_time = timer.elapsed();

		// now update the display
		if(use_region != urNone){
	    st7789->partial_update(graphics, region);
		}
		else {
    	st7789->update(graphics);
		}

		update_time = timer.elapsed();
		total_time  = total_timer.elapsed();

  }

  return 0;
}
