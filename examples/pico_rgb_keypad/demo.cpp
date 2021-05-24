#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_rgb_keypad.hpp"

/*
  Test buttons/lights by lighting them up in a sequence of colors.

  Press each button in turn, it will light up green.
  Once all buttons are lit, they will turn off.
  Press them again to light them in the next color.
*/

using namespace pimoroni;

PicoRGBKeypad pico_keypad;

constexpr uint8_t num_colors = 6;

// Simple struct to pair r/g/b together as a color
struct color {uint8_t r, g, b;};

color colors[num_colors] = {
  {0x00, 0x20, 0x00}, // Green
  {0x20, 0x20, 0x00}, // Yellow
  {0x20, 0x00, 0x00}, // Red
  {0x20, 0x00, 0x20}, // Pink
  {0x00, 0x00, 0x20}, // Blue
  {0x00, 0x20, 0x20}  // Teal
};

int main() {
  pico_keypad.init();
  pico_keypad.set_brightness(1.0f);

  uint16_t lit_buttons = 0;
  uint8_t color_index = 0;
  color current_color = colors[color_index];

  while(true) {
    // Read button states from i2c expander
    // for any pressed buttons set the corresponding bit in "lit_buttons"
    lit_buttons |= pico_keypad.get_button_states();

    // You could use a bitwise OR (^) to make the buttons toggle their respective "lit" bits on and off:
    // lit_buttons ^= pico_keypad.get_button_states();

    // Iterate through the lights
    for(auto i = 0u; i < PicoRGBKeypad::NUM_PADS; i++) {
      if(lit_buttons & (0b1 << i)) {
        pico_keypad.illuminate(i, current_color.r, current_color.g, current_color.b);
      }else{
        // Kinda dim white-ish
        pico_keypad.illuminate(i, 0x05, 0x05, 0x05);
      }
    }

    // Display the LED changes
    pico_keypad.update();

    sleep_ms(100);

    if (lit_buttons == 0xffff) {
      sleep_ms(500); // Wait a little so the last button can be seen
      lit_buttons = 0; // Reset lit buttons
      color_index += 1; // Proceed to the next color
      if(color_index == num_colors) color_index = 0; // Wrap at last color
      current_color = colors[color_index]; // Update the current color
    }
  }

  return 0;
}
