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
#define USE_PICO_EXPLORER 1
// This:
// - Includes pico_explorer.hpp rather than pico_display.hpp
// - Replaces all PicoDisplay references with PicoExplorer
// - Leaves out the .set_led() calls in flash_led()
#ifdef USE_PICO_EXPLORER
#include "pico_explorer.hpp"
#else
#include "pico_display.hpp"
#endif
#include "breakout_tof.hpp"

using namespace pimoroni;

class AutoRepeat {
  public:
    AutoRepeat(uint32_t repeat_time=200, uint32_t hold_time=1000) {
      this->repeat_time = repeat_time;
      this->hold_time = hold_time;
    }
    bool next(uint32_t time, bool state) {
      bool changed = state != last_state;
      last_state = state;

      if(changed) {
        if(state) {
          pressed_time = time;
          pressed = true;
          last_time = time;
          return true;
        }
        else {
          pressed_time = 0;
          pressed = false;
          last_time = 0;
        }
      }
      // Shortcut for no auto-repeat
      if(repeat_time == 0) return false;

      if(pressed) {
        uint32_t repeat_rate = repeat_time;
        if(hold_time > 0 && time - pressed_time > hold_time) {
          repeat_rate /= 3;
        }
        if(time - last_time > repeat_rate) {
          last_time = time;
          return true;
        }
      }

      return false;
    }
  private:
    uint32_t repeat_time;
    uint32_t hold_time;
    bool pressed = false;
    bool last_state = false;
    uint32_t pressed_time = 0;
    uint32_t last_time = 0;
};

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

BreakoutTOF vl53l1x;

const char mode_to_text[4][7] = {
  "Auto",
  "Short",
  "Medium",
  "Long"
};

AutoRepeat ar_button_a;
AutoRepeat ar_button_b;
AutoRepeat ar_button_x;
AutoRepeat ar_button_y;

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

  uint32_t i = 0;
  char buf[256];
  if (vl53_present) {
    vl53l1x.start_continuous(1000);
    vl53l1x.set_measurement_timing_budget(50000);
    vl53l1x.set_distance_mode_int(vl53_mode);
  }

  // The distance (in millimetres)
  uint16_t dist = 0;

  // True if units in metric, false for Imperial (feet and inches)
  bool units_metric = true;

  // Whether the display is being held
  bool dist_held = false;

  while(true) {
    bool a_pressed = ar_button_a.next(i, pico_display.is_pressed(pico_display.A));
    bool b_pressed = ar_button_b.next(i, pico_display.is_pressed(pico_display.B));
    bool x_pressed = ar_button_x.next(i, pico_display.is_pressed(pico_display.X));
    bool y_pressed = ar_button_y.next(i, pico_display.is_pressed(pico_display.Y));

    if (b_pressed) {
      dist_held = !dist_held;
    }

    if (x_pressed) {
      units_metric = !units_metric;
    }

    if (y_pressed) {
      if (vl53_present) {
        vl53_mode++;
        if (vl53_mode > 3) vl53_mode = 1;
        vl53l1x.set_distance_mode_int(vl53_mode);
      }
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
      if(dist_held) {
        pico_display.set_pen(255, 64, 64);
      }
      pico_display.text("Hold",
          Point(text_box.x+disptext_b_reminder_xoff,
            text_box.y+disptext_b_reminder_yoff), 230, disptext_reminder_size);
      pico_display.set_pen(255, 255, 255);

      sprintf(buf, "Mode: %s", mode_to_text[vl53_mode]);
      pico_display.text(buf,
          Point(text_box.x+disptext_mode_xoff,
            text_box.y+disptext_mode_yoff), 230, disptext_mode_size);

      // Get the distance (use previous distance if number is held)
      if (!dist_held) dist = vl53l1x.read(false);
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
