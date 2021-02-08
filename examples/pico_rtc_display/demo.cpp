#include <string.h>

// **************************************************************************
// Demonstrate the Pimoroni RTC module (RV3028)
// Assumes that a Pico Display Pack (a 1.14inch IPS LCD screen with four
// useful buttons) is installed, and that the RV3028 I2C module has
// sda, scl and int on GPIO 20, 21 and 22
// Displays a clock showing date and time (no code yet to set the time,
// so do it on a separate machine), or a countdown timer.
// Button A selects the timer setting function, and runs the timer once set
// Button B selects the clock (the selection on startup)
// Button X increments the timer delay in seconds
// Button Y decrements it
// (There are on-screen reminders of the active buttons)
// **************************************************************************

// To use Explorer,
// - replace pico_display.hpp include with pico_explorer.hpp
// - replace all PicoDisplay with PicoExplorer
// - in CMakeLists.txt replace pico_display with pico_explorer
// - Comment out the .set_led() calls in flash_led()
#include "pico_display.hpp"
#include "rv3028.hpp"

#define MODE_DISP_CLOCK 0
#define MODE_DISP_TIMER 1
#define MODE_SET_TIMER  2

#define DEFAULT_TIMER_COUNT 5

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

RV3028 rv3028;

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
  if ((curr_count % FLASH_MOD) < (FLASH_MOD / 2)) {
    // value less than half modded number - LED off
    pico_display.set_led(0, 0, 0);
  } else {
    // value more than half modded number - LED on
    pico_display.set_led(128, 128, 128);
  }
}

int main() {
  pico_display.init();

  rv3028.init();
  // rv3028.setup(false);

  // Make sure we have 24-hour time (smaller dsplay!)
  if (rv3028.is12Hour()) rv3028.set24Hour();

  // Use these variables to make the buttons single-shot
  // Counts number of loops pressed, 0 if not pressed
  // Only for x and y - a and b are single-shot
  uint16_t a_pressed = 0;
  uint16_t b_pressed = 0;
  uint16_t x_pressed = 0;
  uint16_t y_pressed = 0;

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
  uint8_t display_mode = MODE_DISP_CLOCK;
  uint16_t timer_count = DEFAULT_TIMER_COUNT;

  while(true) {

    if (a_pressed == 0 && pico_display.is_pressed(pico_display.A)) {
      a_pressed = 1;
      if (display_mode == MODE_DISP_CLOCK) {
        // We were displaying clock = set up timer
        display_mode = MODE_SET_TIMER;
      } else if (display_mode == MODE_SET_TIMER) {
        // We were setting up the timer - run it
        rv3028.setTimer(false, 1, timer_count, true, true, false);
        display_mode = MODE_DISP_TIMER;
      } else if (display_mode == MODE_DISP_TIMER) {
        // We were running the timer - go back to setting up
        if (0 == (timer_count = rv3028.getTimerCount())) {
          timer_count = DEFAULT_TIMER_COUNT;
        }
        rv3028.disableTimerInterrupt();
        rv3028.disableTimer();
        rv3028.clearTimerInterruptFlag();
        display_mode = MODE_SET_TIMER;
      }
    } else if (a_pressed >= 1 && !pico_display.is_pressed(pico_display.A)) {
      a_pressed = 0;
    }

    if (b_pressed == 0 && pico_display.is_pressed(pico_display.B)) {
      b_pressed = 1;
      if ((display_mode == MODE_DISP_TIMER)
          || (display_mode == MODE_SET_TIMER)) {
        // We were setting or displaying timer - revert to clock
        rv3028.disableTimerInterrupt();
        rv3028.disableTimer();
        rv3028.clearTimerInterruptFlag();
        display_mode = MODE_DISP_CLOCK;
        timer_count = DEFAULT_TIMER_COUNT;
      }
    } else if (b_pressed >= 1 && !pico_display.is_pressed(pico_display.B)) {
      b_pressed = 0;
    }

    if (x_pressed == 0 && pico_display.is_pressed(pico_display.X)) {
      x_pressed = 1;
      if (display_mode == MODE_SET_TIMER) {
        // Setting timer - Increment count
        timer_count++;
      }
    } else if (x_pressed >= 1 && pico_display.is_pressed(pico_display.X)) {
      // Button still pressed - check if has reached repeat count
      if (repeat_count_reached(x_pressed++)) {
        timer_count++;
      }
    } else if (x_pressed >= 1 && !pico_display.is_pressed(pico_display.X)) {
      x_pressed = 0;
    }

    if (y_pressed == 0 && pico_display.is_pressed(pico_display.Y)) {
      y_pressed = 1;
      if (display_mode == MODE_SET_TIMER) {
        // Setting timer - Decrement count
        if (timer_count >= 1) timer_count--;
      }
    } else if (y_pressed >= 1 && pico_display.is_pressed(pico_display.Y)) {
      // Button still pressed - check if has reached repeat count
      if (repeat_count_reached(y_pressed++)) {
        if (timer_count >= 1) timer_count--;
      }
    } else if (y_pressed >= 1 && !pico_display.is_pressed(pico_display.Y)) {
      y_pressed = 0;
    }

    Rect text_box(5, 5, PicoDisplay::WIDTH-10, PicoDisplay::HEIGHT-10);
    pico_display.set_pen(55, 65, 75);
    pico_display.rectangle(text_box);
    // text_box.deflate(10);
    pico_display.set_clip(text_box);
    pico_display.set_pen(255, 255, 255);
    switch (display_mode) {
      case MODE_DISP_CLOCK:
        // Show the clock face
        flash_led(0);
        if (rv3028.updateTime()) {
          pico_display.text("Set Timer",
              Point(text_box.x, text_box.y+2), 230, 1);
          pico_display.set_pen(0, 255, 0);
          pico_display.text(rv3028.stringDate(),
              Point(text_box.x, text_box.y+20), 230, 4);
          pico_display.set_pen(255, 0, 0);
          pico_display.text(rv3028.stringTime(),
              Point(text_box.x, text_box.y+60), 230, 6);
          pico_display.set_pen(255, 255, 255);
          pico_display.text("Clock",
              Point(text_box.x, text_box.y+PicoDisplay::HEIGHT-20), 230, 1);
        } else {
          sprintf(buf, "Time: rv3028.updateTime() ret err");
          pico_display.text(buf,
              Point(text_box.x, text_box.y), 30, 2);
        }
        break;
      case MODE_DISP_TIMER:
        pico_display.text("Set Timer",
            Point(text_box.x, text_box.y+2), 230, 1);
        if (rv3028.readTimerInterruptFlag()) {
          // Go periodic time interupt - say loop ended
          pico_display.set_pen(255, 0, 0);
          sprintf(buf, "%s", "Timer complete");
          pico_display.text(buf,
              Point(text_box.x, text_box.y+30), 230, 4);
          pico_display.set_pen(255, 255, 255);
          flash_led(i);
        } else {
          sprintf(buf, "%s %d", "Timer running", rv3028.getTimerCount());
          pico_display.text(buf,
              Point(text_box.x, text_box.y+30), 230, 3);
        }
        pico_display.text("Clock",
            Point(text_box.x, text_box.y+PicoDisplay::HEIGHT-20), 230, 1);
        break;
      case MODE_SET_TIMER:
        flash_led(0);
        pico_display.text("Run Timer",
            Point(text_box.x, text_box.y+2), 230, 1);
        pico_display.text("+ Time",
            Point(text_box.x+PicoDisplay::WIDTH-42, text_box.y+2), 230, 1);
        sprintf(buf, "Time %d secs", timer_count);
        pico_display.text(buf,
            Point(text_box.x, text_box.y+30), 230, 3);
        pico_display.text("Clock",
            Point(text_box.x, text_box.y+PicoDisplay::HEIGHT-20), 230, 1);
        pico_display.text("- Time",
            Point(text_box.x+PicoDisplay::WIDTH-42,
              text_box.y+PicoDisplay::HEIGHT-20), 230, 1);
        break;
    }

    pico_display.remove_clip();

    // update screen
    pico_display.update();

    i++;
  }

    return 0;
}
