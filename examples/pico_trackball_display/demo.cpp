#include <string.h>
#include <math.h>

// **************************************************************************
// Demonstrate the Pimoroni Trackball module
// Assumes that a Pico Display Pack (a 1.14inch IPS LCD screen with four
// useful buttons) is installed, and that the Trackball I2C module has
// sda, scl and int on GPIO 20, 21 and 22
// Displays a set of coloured circles and a small cursor.
// Use the trackball to move the cursor around. The LED of the trackball
// will become the colour of whatever circle the cursosr is under.
// Pressing in on the trackball whilst over a circle will cause it to
// appear "pressed". Doing so on a "pressed" circle will make it "unpressed".
// **************************************************************************

// To use PicoExplorer rather than PicoDisplay, uncomment the following line
#define USE_PICO_EXPLORER 1
// This:
// - Includes pico_explorer.hpp rather than pico_display.hpp
// - Replaces all PicoDisplay references with PicoExplorer
#ifdef USE_PICO_EXPLORER
#include "pico_explorer.hpp"
#else
#include "pico_display.hpp"
#endif
#include "breakout_trackball.hpp"

using namespace pimoroni;

struct TrackballColour {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

#ifdef USE_PICO_EXPLORER
uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_display(buffer);
const uint16_t screen_width = PicoExplorer::WIDTH;
const uint16_t screen_height = PicoExplorer::HEIGHT;
#else
uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);
const uint16_t screen_width = PicoDisplay::WIDTH;
const uint16_t screen_height = PicoDisplay::HEIGHT;
#endif
const Point screen_centre(screen_width / 2, screen_height / 2);
const uint16_t circle_radius = std::min(screen_centre.x, screen_centre.y) / 4;
const float ring_radius_mult = 0.7f;
const uint16_t circle_border = 4;
const uint16_t cursor_radius = 8;
const uint16_t cursor_border = 4;


BreakoutTrackball trackball;

const uint8_t NUM_CIRCLES = 6;
TrackballColour colour_circles[NUM_CIRCLES] = {
  { 255, 0, 0, 0 },
  { 255, 255, 0, 0 },
  { 0, 255, 0, 0 },
  { 0, 255, 255, 0 },
  { 0, 0, 255, 0 },
  { 255, 0, 255, 0 },
};
bool circle_states[NUM_CIRCLES] = { false, false, false, false, false, false };
bool centre_circle_state = false;


int main() {
  int16_t x = screen_centre.x;
  int16_t y = screen_centre.y;
  pico_display.init();

  trackball.init();

  Point positions[NUM_CIRCLES];
  for(uint8_t i = 0; i < NUM_CIRCLES; i++) {
    float angle_rad = (i * 2 * M_PI) / NUM_CIRCLES;
    Point pos(screen_centre.x + (sinf(angle_rad) * screen_centre.x * ring_radius_mult),
              screen_centre.y + (cosf(angle_rad) * screen_centre.y * ring_radius_mult));

    positions[i] = pos;
  }

  while(true) {
    Trackball::State state = trackball.read();
    x = std::min(std::max(x - state.left + state.right, 0), (int)screen_width);
    y = std::min(std::max(y - state.up + state.down, 0), (int)screen_height);
    Point cursor_pos(x, y);

    pico_display.set_pen(0, 0, 0);
    pico_display.clear();

    //Draw a set of circles in a ring around the screen centre
    for(uint8_t i = 0; i < NUM_CIRCLES; i++) {
      TrackballColour col = colour_circles[i];

      if(circle_states[i]) {
        pico_display.set_pen(col.r, col.g, col.b);
        pico_display.circle(positions[i], circle_radius + circle_border);
        pico_display.set_pen(col.r >> 1, col.g >> 1, col.b >> 1);
        pico_display.circle(positions[i], circle_radius);
      }
      else {
        pico_display.set_pen(col.r >> 1, col.g >> 1, col.b >> 1);
        pico_display.circle(positions[i], circle_radius + circle_border);
        pico_display.set_pen(col.r, col.g, col.b);
        pico_display.circle(positions[i], circle_radius);
      }
    }

    //Draw a centre circle
    if(centre_circle_state) {
      pico_display.set_pen(255, 255, 255);
      pico_display.circle(screen_centre, circle_radius + circle_border);
      pico_display.set_pen(128, 128, 128);
      pico_display.circle(screen_centre, circle_radius);
    }
    else {
      pico_display.set_pen(128, 128, 128);
      pico_display.circle(screen_centre, circle_radius + circle_border);
      pico_display.set_pen(255, 255, 255);
      pico_display.circle(screen_centre, circle_radius);
    }

    //Draw the cursor
    pico_display.set_pen(0, 0, 0);
    pico_display.circle(cursor_pos, cursor_radius + cursor_border);
    pico_display.set_pen(212, 212, 212);
    pico_display.circle(cursor_pos, cursor_radius);

    int16_t x_diff = cursor_pos.x - screen_centre.x;
    int16_t y_diff = cursor_pos.y - screen_centre.y;
    if((x_diff * x_diff) + (y_diff * y_diff) < circle_radius * circle_radius) {
      trackball.set_rgbw(0, 0, 0, 255);
      if(state.sw_changed && state.sw_pressed)
        centre_circle_state = !centre_circle_state;
    }
    else {
      bool colour_set = false;
      for(uint8_t i = 0; i < NUM_CIRCLES; i++) {
        uint8_t inv_i = NUM_CIRCLES - i - 1;

        TrackballColour col = colour_circles[inv_i];
        x_diff = cursor_pos.x - positions[inv_i].x;
        y_diff = cursor_pos.y - positions[inv_i].y;
        if((x_diff * x_diff) + (y_diff * y_diff) < circle_radius * circle_radius) {
          trackball.set_rgbw(col.r, col.g, col.b, col.w);
          colour_set = true;
          if(state.sw_changed && state.sw_pressed)
            circle_states[inv_i] = !circle_states[inv_i];
          break;
        }
      }

      if(!colour_set) {
        trackball.set_rgbw(0, 0, 0, 0);
      }
    }

    // update screen
    pico_display.update();
  }

  return 0;
}
