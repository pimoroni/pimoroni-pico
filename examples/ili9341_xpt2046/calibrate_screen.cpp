// class for a simple calibrate screen

#include "calibrate_screen.hpp"
#include "elapsed_us.hpp"

#define BUTTON_HEIGHT (32)

using namespace pimoroni;

#include <numeric> 

// Simple calibrate screen
// First press against screen varying pressure multiple times till countdown hits 0
// Displays a cross, tap on the center
// Displays another cross, tap on the center.
// Then test the screen.
// Wait 3 seconds without touching
// If everything is good tap the button at the bottom otherwise wait 3 seconds to retry.

void Calibrate_Screen::run_calibration(ILI9341* ili9341, XPT2046* xpt2046, PicoGraphics* graphics) 
{
  enum CalibrateState {csPressure, csTopLeft, csTopLeftScan, csBottomRight, csBottomRightScan, csTest, csTestCountdown, csAllOk, csExit};

  CalibrateState state = csPressure;

  TouchPoint top_left;
  TouchPoint bottom_right;
  uint16_t min_pressure = 0xffff;
  uint16_t max_pressure = 0;
  
  uint s = 20;
  uint count_pressure_readings = 100;

  ElapsedUs countdown_timer;

  char log_buffer[64];

  while(state != csExit) {
    if(ili9341->is_using_async_dma()) {
      ili9341->wait_for_update_to_finish();
    }

    xpt2046->update();
    switch(state)
    {
      case csPressure: {
        
        // disable z handling
        xpt2046->set_z_enabled(false);

        sprintf(log_buffer, "Vary pressure (%u)", count_pressure_readings);
        draw_calibrate_background(graphics, log_buffer);
        if(xpt2046->is_touched())	{
          count_pressure_readings--;
          TouchPoint tp = xpt2046->get_raw_touch();
          
          if(tp.z < min_pressure) {
            min_pressure = tp.z;
          }
          
          if(tp.z > max_pressure) {
            max_pressure = tp.z;
          }
        }

        if(count_pressure_readings == 0) {
          // Calibrate Z
          xpt2046->calibrate_z(min_pressure, max_pressure);

          // enable z handling
          xpt2046->set_z_enabled(true);

          state = csTopLeft;
        }
        break;
      }

      case csTopLeft: {
        draw_calibrate_background(graphics, "Touch cross");

        switch(xpt2046->get_rotation())
        {
          case ROTATE_0   : draw_cross(graphics,s,graphics->bounds.h-s-1,s); break;
          case ROTATE_90  : draw_cross(graphics,graphics->bounds.w-1-s, graphics->bounds.h-1-s,s); break;
          case ROTATE_180 : draw_cross(graphics,graphics->bounds.w-1-s, s, 20); break;
          case ROTATE_270 : draw_cross(graphics,s, s, s); break;
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
        draw_calibrate_background(graphics, "Touch cross");

        switch(xpt2046->get_rotation())
        {
          case ROTATE_0   : draw_cross(graphics, graphics->bounds.w-1-s, s, 20); break;
          case ROTATE_90  : draw_cross(graphics, s, s, s); break;
          case ROTATE_180 : draw_cross(graphics, s,graphics->bounds.h-s-1,s); break;
          case ROTATE_270 : draw_cross(graphics, graphics->bounds.w-1-s, graphics->bounds.h-1-s,s); break;
        }
        state = csBottomRightScan;
        break;
      }

      case csBottomRightScan: {
        bottom_right = get_raw_touch(xpt2046);
        printf("calibrate_touchscreen(TouchPoint(%lu, %lu), TouchPoint(%lu, %lu), %u, %u, %u);\n", top_left.x, top_left.y, bottom_right.x, bottom_right.y, min_pressure, max_pressure,  s);
        xpt2046->calibrate_touchscreen(top_left, bottom_right, min_pressure, max_pressure, s);
        state = csTest;
        break;
      }

      case csTest: {

        draw_calibrate_background(graphics, "Test screen");

        if(xpt2046->is_touched()) {
          TouchPoint p = xpt2046->get_touch();
          graphics->line(Point(p.x, 0), Point(p.x, graphics->bounds.h));
          graphics->line(Point(0, p.y), Point(graphics->bounds.w, p.y));
          graphics->circle(Point(p.x, p.y), 1+(p.z/5));
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
            sprintf(log_buffer, "Test Screen Please (%u)", secs_remaining);
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
          sprintf(log_buffer, "Everying Ok? (%u)", secs_remaining);
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
            TouchPoint p = xpt2046->get_touch();

            if(p.y > graphics->bounds.h - BUTTON_HEIGHT) {
                state = csExit;
            }
          }
        }
        else {
          count_pressure_readings = 100;
          state = csPressure;
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
TouchPoint Calibrate_Screen::get_raw_touch(XPT2046* xpt2046) {
  TouchPoint result;

  // wait for any touch to finish
  while(xpt2046->is_touched()) {
    xpt2046->update(256);
  }

  // wait for touch down
  while(!xpt2046->is_touched()) {
    xpt2046->update(256);
  }

  // wait for touch up
  while(xpt2046->is_touched()) {
    xpt2046->update(256);
  }

  result = xpt2046->get_raw_touch();	

  return result;
}
