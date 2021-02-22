#include <string.h>

// **************************************************************************
// Demonstrate the Pimoroni ToF module (VL53L1X)
// Assumes that a Pico Display Pack (a 1.14inch IPS LCD screen with four
// useful buttons) is installed, and that the VL53L1X I2C module has
// sda, scl and int on GPIO 20, 21 and 22
// Displays the current distance (in mm) and mode
// Button B holds the reading until released
// Button X toggles metric/imperial display
// Button Y increments the mode (Auto, Short, Medium, Long)
// (There are on-screen reminders of the active buttons)
// **************************************************************************

// To use PicoExplorer rather than PicoDisplay, uncomment the following line
// #define USE_PICO_EXPLORER 1
// This:
// - Includes pico_explorer.hpp rather than pico_display.hpp
// - Replaces all PicoDisplay references with PicoExplorer
// - Leaves out the .set_led() calls in flash_led()
#ifdef USE_PICO_EXPLORER
#include "pico_explorer.hpp"
#else
#include "pico_display.hpp"
#endif
#include "vl53l1x.hpp"

using namespace pimoroni;

#ifdef USE_PICO_EXPLORER
uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_display(buffer);
uint16_t screen_width = PicoExplorer::WIDTH;
uint16_t screen_height = PicoExplorer::HEIGHT;
uint16_t disptext_reminder_size = 2;
uint16_t disptext_b_reminder_xoff = 5;
uint16_t disptext_b_reminder_yoff = 210;
uint16_t disptext_x_reminder_xoff = 165;
uint16_t disptext_x_reminder_yoff = 2;
uint16_t disptext_y_reminder_xoff = 165;
uint16_t disptext_y_reminder_yoff = 210;
uint16_t disptext_mode_xoff = 10;
uint16_t disptext_mode_yoff = 30;
uint16_t disptext_mode_size = 3;
uint16_t disptext_dist_xoff = 10;
uint16_t disptext_dist_yoff = 90;
uint16_t disptext_dist_size = 6;
#else
uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);
uint16_t screen_width = PicoDisplay::WIDTH;
uint16_t screen_height = PicoDisplay::HEIGHT;
uint16_t disptext_reminder_size = 2;
uint16_t disptext_b_reminder_xoff = 2;
uint16_t disptext_b_reminder_yoff = 110;
uint16_t disptext_x_reminder_xoff = 175;
uint16_t disptext_x_reminder_yoff = 2;
uint16_t disptext_y_reminder_xoff = 175;
uint16_t disptext_y_reminder_yoff = 110;
uint16_t disptext_mode_xoff = 12;
uint16_t disptext_mode_yoff = 15;
uint16_t disptext_mode_size = 3;
uint16_t disptext_dist_xoff = 10;
uint16_t disptext_dist_yoff = 45;
uint16_t disptext_dist_size = 4;
#endif

#define MM_TO_INCH 25.4

VL53L1X vl53l1x;

char * mode_to_text[4];

#define LOW_COUNT_MOD 40
#define HIGH_COUNT_MOD 20
bool repeat_count_reached(uint16_t curr_count) {
  // Check whether the current counter means that a key has repeated
  if (curr_count <= 10*LOW_COUNT_MOD) {
    return (0 == (curr_count % LOW_COUNT_MOD));
  } else {
    return (0 == (curr_count % HIGH_COUNT_MOD));
  }
}

#define FLASH_MOD 20
void flash_led(uint32_t curr_count) {
  // Flash the LED based on the current loop counter
  // curr_count=0 will turn LED off
#ifndef USE_PICO_EXPLORER
  if ((curr_count % FLASH_MOD) < (FLASH_MOD / 2)) {
    // value less than half modded number - LED off
    pico_display.set_led(0, 0, 0);
  } else {
    // value more than half modded number - LED on
    pico_display.set_led(128, 128, 128);
  }
#endif
}

int main() {
  bool vl53_present = false;
  uint16_t vl53_mode = 1;

  pico_display.init();

  vl53_present = vl53l1x.init();

  // Use these variables to make the buttons single-shot
  // Counts number of loops pressed, 0 if not pressed
  // Only for x and y - a and b are single-shot
  uint16_t a_pressed = 0;
  uint16_t b_pressed = 0;
  uint16_t x_pressed = 0;
  uint16_t y_pressed = 0;

  mode_to_text[0] = "Auto";
  mode_to_text[1] = "Short";
  mode_to_text[2] = "Medium";
  mode_to_text[3] = "Long";

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  uint32_t i = 0;
  char buf[256];
  if (vl53_present) {
    vl53l1x.startContinuous(1000);
    vl53l1x.setMeasurementTimingBudget(50000);
    vl53l1x.setDistanceModeInt(vl53_mode);
  }

  // The distance (in millimetres)
  uint16_t dist = 0;

  // True if units in metric, false for Imperial (feet and inches)
  bool units_metric = true;

  // Whether the display is being held
  bool dist_held = false;

  while(true) {

    if (a_pressed == 0 && pico_display.is_pressed(pico_display.A)) {
      a_pressed = 1;
    } else if (a_pressed >= 1 && !pico_display.is_pressed(pico_display.A)) {
      a_pressed = 0;
    }

    if (b_pressed == 0 && pico_display.is_pressed(pico_display.B)) {
      b_pressed = 1;
      dist_held = true;
    } else if (b_pressed >= 1 && !pico_display.is_pressed(pico_display.B)) {
      b_pressed = 0;
      dist_held = false;
    }

    if (x_pressed == 0 && pico_display.is_pressed(pico_display.X)) {
      x_pressed = 1;
      units_metric = !units_metric;
    } else if (x_pressed >= 1 && pico_display.is_pressed(pico_display.X)) {
      // Button still pressed - check if has reached repeat count
      if (repeat_count_reached(x_pressed++)) {
        // Do nothing for now - may need this later!
      }
    } else if (x_pressed >= 1 && !pico_display.is_pressed(pico_display.X)) {
      x_pressed = 0;
    }

    if (y_pressed == 0 && pico_display.is_pressed(pico_display.Y)) {
      y_pressed = 1;
      if (vl53_present) {
        vl53_mode++;
        if (vl53_mode > 3) vl53_mode = 1;
        vl53l1x.setDistanceModeInt(vl53_mode);
      }
    } else if (y_pressed >= 1 && pico_display.is_pressed(pico_display.Y)) {
      // Button still pressed - check if has reached repeat count
      if (repeat_count_reached(y_pressed++)) {
        // Do nothing for now - may need this later!
      }
    } else if (y_pressed >= 1 && !pico_display.is_pressed(pico_display.Y)) {
      y_pressed = 0;
    }

    Rect text_box(5, 5, screen_width-10, screen_height-10);
    pico_display.set_pen(55, 65, 75);
    pico_display.rectangle(text_box);
    // text_box.deflate(10);
    pico_display.set_clip(text_box);
    pico_display.set_pen(255, 255, 255);
    // Show the current distance
    flash_led(0);
    if (vl53_present) {
      pico_display.text("Units",
          Point(text_box.x+disptext_x_reminder_xoff,
            text_box.y+disptext_x_reminder_yoff), 230, disptext_reminder_size);
      pico_display.text("+Mode",
          Point(text_box.x+disptext_y_reminder_xoff,
            text_box.y+disptext_y_reminder_yoff), 230, disptext_reminder_size);
      pico_display.text("Hold",
          Point(text_box.x+disptext_b_reminder_xoff,
            text_box.y+disptext_b_reminder_yoff), 230, disptext_reminder_size);

      sprintf(buf, "Mode: %s", mode_to_text[vl53_mode]);
      pico_display.text(buf,
          Point(text_box.x+disptext_mode_xoff,
            text_box.y+disptext_mode_yoff), 230, disptext_mode_size);

      // Get the distance (use previous distance if number is held)
      if (!dist_held) dist = vl53l1x.read();
      if (units_metric) {
        sprintf(buf, "%dmm", dist);
      } else {
        uint16_t ft = ((uint16_t)(dist/MM_TO_INCH))/12;
        sprintf(buf, "%dft %.1fin", ft,
            ((float)dist/MM_TO_INCH)-ft*12.0);
      }
      pico_display.text(buf,
          Point(text_box.x+disptext_dist_xoff,
            text_box.y+disptext_dist_yoff), 120, disptext_dist_size);
    } else {
      pico_display.text("VL53L1X Missing",
          Point(text_box.x+disptext_dist_xoff,
            text_box.y+disptext_dist_yoff), 230, disptext_dist_size);
    }

    pico_display.remove_clip();

    // update screen
    pico_display.update();

    i++;
  }

    return 0;
}
