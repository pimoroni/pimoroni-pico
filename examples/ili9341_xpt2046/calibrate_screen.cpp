// class for a simple calibrate screen

#include "calibrate_screen.hpp"
#include "elapsed_us.hpp"

#define BUTTON_HEIGHT (32)

using namespace pimoroni;

// Simple calibrate screen
// Displays a cross, tap on the center
// Displays another cross, tap on the center.
// Then test the screen.
// Wait 3 seconds without touching
// If everything is good tap the button at the bottom otherwise wait 3 seconds to retry.

void Calibrate_Screen::run_calibration(ILI9341* ili9341, XPT2046* xpt2046, PicoGraphics* graphics) 
{
	enum CalibrateState {csTopLeft, csTopLeftScan, csBottomRight, csBottomRightScan, csTest, csTestCountdown, csAllOk, csExit};

	CalibrateState state = csTopLeft;

	Point top_left;
	Point bottom_right;

	uint s = 20;

	ElapsedUs countdown_timer;

	char log_buffer[64];

	while(state != csExit) {
		if(ili9341->is_using_async_dma()) {
			ili9341->wait_for_update_to_finish();
		}

		xpt2046->update();
		switch(state)
		{
			case csTopLeft: {
				draw_calibrate_background(graphics, "Touch center of cross");

				switch(xpt2046->get_rotation())
				{
					case ROTATE_0   : draw_cross(graphics,s, s, s); break;
					case ROTATE_90  : draw_cross(graphics,s,graphics->bounds.h-s-1,s); break;
					case ROTATE_180 : draw_cross(graphics,graphics->bounds.w-1-s, graphics->bounds.h-1-s,s); break;
					case ROTATE_270 : draw_cross(graphics,graphics->bounds.w-1-s, s, 20); break;
				}
				state = csTopLeftScan;
				break;
			}
			
			case csTopLeftScan: {
				top_left = Calibrate_Screen::get_raw_touch(xpt2046);
				state = csBottomRight;
				break;
			}

			case csBottomRight: {
				draw_calibrate_background(graphics, "Touch center of cross");

				switch(xpt2046->get_rotation())
				{
					case ROTATE_0   : draw_cross(graphics, graphics->bounds.w-1-s, graphics->bounds.h-1-s,s); break;
					case ROTATE_90  : draw_cross(graphics, graphics->bounds.w-1-s, s, 20); break;
					case ROTATE_180 : draw_cross(graphics, s, s, s); break;
					case ROTATE_270 : draw_cross(graphics, s,graphics->bounds.h-s-1,s); break;
				}
				state = csBottomRightScan;
				break;
			}

			case csBottomRightScan: {
				bottom_right = get_raw_touch(xpt2046);
				printf("xpt2046->calibrate_touchscreen(Point(%lu, %lu), Point(%lu, %lu). %u)\n", top_left.x, top_left.y, bottom_right.x, bottom_right.y, s);
				xpt2046->calibrate_touchscreen(top_left, bottom_right, s);
				state = csTest;
				break;
			}

			case csTest: {

				draw_calibrate_background(graphics, "Test screen please");

				if(xpt2046->is_touched()) {
					Point p = xpt2046->get_touch();
					graphics->line(Point(p.x, 0), Point(p.x, graphics->bounds.h));
					graphics->line(Point(0, p.y), Point(graphics->bounds.w, p.y));
				}
				else {
					countdown_timer.reset();
					state = csTestCountdown;
				}
				
				break;
			}

			case csTestCountdown: {

				if(xpt2046->is_touched()) {
					state = csTest;
				}
				else {
					uint secs_remaining = 3 - (countdown_timer.elapsed(false) / 1000000);
					if(secs_remaining != 0) {
						sprintf(log_buffer, "Test Screen Please (%x)", secs_remaining);
						draw_calibrate_background(graphics, log_buffer);
					}
					else {
						countdown_timer.reset();
						state = csAllOk;
					}
				}

				break;
			}

			case csAllOk: {
				uint secs_remaining = 3 - (countdown_timer.elapsed(false) / 1000000);
				if(secs_remaining != 0) {
					sprintf(log_buffer, "Everying Ok? (%x)", secs_remaining);
					draw_calibrate_background(graphics, log_buffer);

					uint button_width  = graphics->bounds.w-1;
					uint button_height = BUTTON_HEIGHT;
					uint y1 = graphics->bounds.h - button_height;

					const char* str = "Everything Ok";

					Rect r(0, y1, button_width, button_height);
					graphics->rectangle_frame(r);
					int32_t text_width = graphics->measure_text(str);
					uint offset = (button_width - text_width)/2;
					graphics->text(str, Point(r.x+offset, r.y+8), r.x+button_width, 2);

					if(xpt2046->is_touched()) {
						Point p = xpt2046->get_touch();

						if(p.y > graphics->bounds.h - BUTTON_HEIGHT) {
								state = csExit;
						}
					}
				}
				else {
					state = csTopLeft;
				}
				break;
			}

			case csExit : break;
		}

		// update full screen
		ili9341->update(graphics);
	}
}

// Draw a cross
void Calibrate_Screen::draw_cross(PicoGraphics* graphics, uint x, uint y, uint size) {
	graphics->line(Point(x,y-size), Point(x, y+size));
	graphics->line(Point(x-size,y), Point(x+size, y));
}

// Draw the background for the calibration process
void Calibrate_Screen::draw_calibrate_background(PicoGraphics* graphics, const char* str) {
	graphics->set_pen(0, 0, 0);
	graphics->clear();
	
	int32_t text_width = graphics->measure_text(str);
	uint x = (graphics->bounds.w / 2) - (text_width /2);
	uint y = (graphics->bounds.h / 2) - 16;

	graphics->set_pen(255, 255, 255);
	graphics->text(str, Point(x, y), 256, 2);
}

// Gets a raw touch for the calibration process
Point Calibrate_Screen::get_raw_touch(XPT2046* xpt2046) {
	Point result;

	// wait for any touch to finish
	while(xpt2046->is_touched()) {
		xpt2046->update();
	}

	// wait for touch down
	while(!xpt2046->is_touched()) {
		xpt2046->update();
	}

	// wait for touch up
	while(xpt2046->is_touched()) {
		xpt2046->update();
	}

	result = xpt2046->get_raw_touch();	

	return result;
}
