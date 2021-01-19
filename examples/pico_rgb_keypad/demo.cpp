#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad pico_keypad;

int main() {

  pico_keypad.init();
  pico_keypad.set_brightness(1.0f);

  uint16_t lit = 0;
  uint16_t last_button_states = 0;
  uint8_t colour_index = 0;

  while(true) {
    // read button states from i2c expander
    uint16_t button_states = pico_keypad.get_button_states();

    if(last_button_states != button_states && button_states) {
      last_button_states = button_states;
      if(button_states) {
        if(lit == 0xffff) {
          // all buttons are already lit, reset the test
          lit = 0;
          colour_index++;
          if(colour_index >= 6) {
            colour_index = 0;
          }
        }else{
          uint8_t button = 0;
          for(uint8_t find = 0; find < pico_keypad.NUM_PADS; find++) {
            // check if this button is pressed and no other buttons are pressed
            if(button_states & 0x01) {
              if(!(button_states & (~0x01))) {
                lit |= 1 << button;
              }
              break;
            }
            button_states >>= 1;
            button++;
          }
        }
      }
    }

    last_button_states = button_states;

    for(uint8_t i = 0; i < PicoRGBKeypad::NUM_PADS; i++) {
      if((lit >> i) & 0x01) {
        switch(colour_index)
        {
          case 0: pico_keypad.illuminate(i, 0x00, 0x20, 0x00);  break;
          case 1: pico_keypad.illuminate(i, 0x20, 0x20, 0x00);  break;
          case 2: pico_keypad.illuminate(i, 0x20, 0x00, 0x00);  break;
          case 3: pico_keypad.illuminate(i, 0x20, 0x00, 0x20);  break;
          case 4: pico_keypad.illuminate(i, 0x00, 0x00, 0x20);  break;
          case 5: pico_keypad.illuminate(i, 0x00, 0x20, 0x20);  break;
        }
      }else{
        pico_keypad.illuminate(i, 0x05, 0x05, 0x05);
      }
    }

    pico_keypad.update();

    sleep_ms(100);
  }

  return 0;
}
