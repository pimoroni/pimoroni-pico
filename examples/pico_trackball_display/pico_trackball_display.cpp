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
// - Includes pico_explorer.hpp rather than display.hpp
// - Replaces all PicoDisplay references with PicoExplorer
#ifdef USE_PICO_EXPLORER
#include "pico_explorer.hpp"
#else
#include "pico_display.hpp"
#endif
#include "breakout_trackball.hpp"

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

struct TrackballColour {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

#ifdef USE_PICO_EXPLORER
const uint16_t screen_width = PicoExplorer::WIDTH;
const uint16_t screen_height = PicoExplorer::HEIGHT;
#else
const uint16_t screen_width = PicoDisplay::WIDTH;
const uint16_t screen_height = PicoDisplay::HEIGHT;
#endif

ST7789 st7789(screen_width, screen_height, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

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

  trackball.init();

  Point positions[NUM_CIRCLES];
  for(uint8_t i = 0; i < NUM_CIRCLES; i++) {
    float angle_rad = (i * 2 * M_PI) / NUM_CIRCLES;
    Point pos(screen_centre.x + (sinf(angle_rad) * screen_centre.x * ring_radius_mult),
              screen_centre.y + (cosf(angle_rad) * screen_centre.y * ring_radius_mult));

    positions[i] = pos;
  }

  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen BLACK = graphics.create_pen(0, 0, 0);
  Pen LIGHT_GREY = graphics.create_pen(212, 212, 212);
  Pen MID_GREY = graphics.create_pen(128, 128, 128);

  while(true) {
    Trackball::State state = trackball.read();
    x = std::min(std::max(x - state.left + state.right, 0), (int)screen_width);
    y = std::min(std::max(y - state.up + state.down, 0), (int)screen_height);
    Point cursor_pos(x, y);

    graphics.set_pen(BLACK);
    graphics.clear();

    //Draw a set of circles in a ring around the screen centre
    for(uint8_t i = 0; i < NUM_CIRCLES; i++) {
      TrackballColour col = colour_circles[i];

      if(circle_states[i]) {
        graphics.set_pen(graphics.create_pen(col.r, col.g, col.b));
        graphics.circle(positions[i], circle_radius + circle_border);
        graphics.set_pen(graphics.create_pen(col.r >> 1, col.g >> 1, col.b >> 1));
        graphics.circle(positions[i], circle_radius);
      }
      else {
        graphics.set_pen(graphics.create_pen(col.r >> 1, col.g >> 1, col.b >> 1));
        graphics.circle(positions[i], circle_radius + circle_border);
        graphics.set_pen(graphics.create_pen(col.r, col.g, col.b));
        graphics.circle(positions[i], circle_radius);
      }
    }

    //Draw a centre circle
    if(centre_circle_state) {
      graphics.set_pen(WHITE);
      graphics.circle(screen_centre, circle_radius + circle_border);
      graphics.set_pen(MID_GREY);
      graphics.circle(screen_centre, circle_radius);
    }
    else {
      graphics.set_pen(MID_GREY);
      graphics.circle(screen_centre, circle_radius + circle_border);
      graphics.set_pen(WHITE);
      graphics.circle(screen_centre, circle_radius);
    }

    //Draw the cursor
    graphics.set_pen(BLACK);
    graphics.circle(cursor_pos, cursor_radius + cursor_border);
    graphics.set_pen(LIGHT_GREY);
    graphics.circle(cursor_pos, cursor_radius);

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
    st7789.update(&graphics);
  }

  return 0;
}
