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
#include "vl53l1x.hpp"
#include "drivers/button/button.hpp"
#include "drivers/rgbled/rgbled.hpp"

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

#ifdef USE_PICO_EXPLORER
ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));

Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer::B);
Button button_x(PicoExplorer::X);
Button button_y(PicoExplorer::Y);

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
ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));

RGBLED rgbled(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);

Button button_a(PicoDisplay::A);
Button button_b(PicoDisplay::B);
Button button_x(PicoDisplay::X);
Button button_y(PicoDisplay::Y);

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

PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

uint16_t screen_width = graphics.bounds.w;
uint16_t screen_height = graphics.bounds.h;

#define MM_TO_INCH 25.4

VL53L1X vl53l1x;

const char mode_to_text[4][7] = {
  "Auto",
  "Short",
  "Medium",
  "Long"
};

#define FLASH_MOD 20
void flash_led(uint32_t curr_count) {
  // Flash the LED based on the current loop counter
  // curr_count=0 will turn LED off
#ifndef USE_PICO_EXPLORER
  if ((curr_count % FLASH_MOD) < (FLASH_MOD / 2)) {
    // value less than half modded number - LED off
    rgbled.set_rgb(0, 0, 0);
  } else {
    // value more than half modded number - LED on
    rgbled.set_rgb(128, 128, 128);
  }
#endif
}

int main() {
  bool vl53_present = false;
  uint16_t vl53_mode = 1;

  vl53_present = vl53l1x.init();

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

  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen REDDISH = graphics.create_pen(255, 64, 64);
  Pen BG = graphics.create_pen(55, 65, 75);

  while(true) {
    // bool a_pressed = button_a.read();
    bool b_pressed = button_b.read();
    bool x_pressed = button_x.read();
    bool y_pressed = button_y.read();

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
        vl53l1x.setDistanceModeInt(vl53_mode);
      }
    }

    Rect text_box(5, 5, screen_width-10, screen_height-10);
    graphics.set_pen(BG);
    graphics.rectangle(text_box);
    // text_box.deflate(10);
    graphics.set_clip(text_box);
    graphics.set_pen(WHITE);
    // Show the current distance
    flash_led(0);
    if (vl53_present) {
      graphics.text("Units",
          Point(text_box.x+disptext_x_reminder_xoff,
            text_box.y+disptext_x_reminder_yoff), 230, disptext_reminder_size);
      graphics.text("+Mode",
          Point(text_box.x+disptext_y_reminder_xoff,
            text_box.y+disptext_y_reminder_yoff), 230, disptext_reminder_size);
      if(dist_held) {
        graphics.set_pen(REDDISH);
      }
      graphics.text("Hold",
          Point(text_box.x+disptext_b_reminder_xoff,
            text_box.y+disptext_b_reminder_yoff), 230, disptext_reminder_size);
      graphics.set_pen(WHITE);

      sprintf(buf, "Mode: %s", mode_to_text[vl53_mode]);
      graphics.text(buf,
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
      graphics.text(buf,
          Point(text_box.x+disptext_dist_xoff,
            text_box.y+disptext_dist_yoff), 120, disptext_dist_size);
    } else {
      graphics.text("VL53L1X Missing",
          Point(text_box.x+disptext_dist_xoff,
            text_box.y+disptext_dist_yoff), 230, disptext_dist_size);
    }

    graphics.remove_clip();

    // update screen
    st7789.update(&graphics);

    i++;
  }

    return 0;
}
