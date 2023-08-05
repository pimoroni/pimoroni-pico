#include "pico/stdlib.h"

#include "yukon.hpp"
using namespace pimoroni;
/*
Demonstrates how to create a Servo object and control it.
*/

const std::string OUTPUT_1_NAME = "Small Fan";
const std::string OUTPUT_2_NAME = "Big Fan";
const uint SLOT_ID = 1;

Yukon yukon(12.1f);

#define CHAR_CTRL_C (3)
#define CHAR_CTRL_D (4)

int last_char = PICO_ERROR_TIMEOUT;

bool check_for_ctrlc() {
  int c = getchar_timeout_us(0);
  if(c == CHAR_CTRL_C) {
    printf("CTRL-C\n");
    return true;
  }
  return false;
}

int main() {
  stdio_init_all();
  yukon.change_logging(3);

  try {
    // Initialise the servo
    yukon.init();

    while (!stdio_usb_connected()) {
      sleep_ms(100);
    }
    while(!check_for_ctrlc());
    while(!check_for_ctrlc());
    printf("stdio_usb_connected()\n");

    // Create a DualSwitchedModule and register it with a slot on Yukon
    DualSwitchedModule switches;
    yukon.register_with_slot(switches, SLOT_ID);

    // Initialise Yukon's registered modules
    yukon.initialise_modules();

    // Turn on the module power
    yukon.enable_main_output();

    // Enable the switched outputs
    switches.enable(1);
    switches.enable(2);

    //uint offset = 0;
    //bool sw_a_state = false;
    //bool sw_b_state = false;
    bool last_sw_a_state = false;
    bool last_sw_b_state = false;

    while(!yukon.is_boot_pressed()) {
      bool sw_a_state = yukon.is_pressed(0);
      bool sw_b_state = yukon.is_pressed(1);

      if(sw_a_state && sw_a_state != last_sw_a_state) {
        bool new_state = !switches.read_output(1);
        switches.output(1, new_state);
        yukon.set_led(0, new_state);
        if(new_state)
          logging.print(OUTPUT_1_NAME + " = On\n");
        else
          logging.print(OUTPUT_1_NAME + " = Off\n");
      }

      if(sw_b_state && sw_b_state != last_sw_b_state) {
        bool new_state = not switches.read_output(2);
        switches.output(2, new_state);
        yukon.set_led(1, new_state);
        if(new_state)
          logging.print(OUTPUT_2_NAME + " = On\n");
        else
          logging.print(OUTPUT_2_NAME + " = Off\n");
      }

      last_sw_a_state = sw_a_state;
      last_sw_b_state = sw_b_state;

      // Use the Yukon class to sleep, which lets it monitor voltage, current, and temperature
      yukon.monitored_sleep_ms(100);

      if(stdio_usb_connected()) {
        int c = getchar_timeout_us(0);
        if(c != PICO_ERROR_TIMEOUT) {
          if(c == CHAR_CTRL_C) {
            printf("CTRL-C\n");
            if(last_char == CHAR_CTRL_C) {
              printf("exiting\n");
              break;
            }
          }
          if(c == CHAR_CTRL_D) {
            printf("CTRL-D\n");
            printf("exiting\n");
            break;
          }
          last_char = c;
        }
      } 
    }
  }
  catch(const std::exception &e) {
    printf(e.what());
  }

  // Put the board back into a safe state, regardless of how the program may have ended
  yukon.reset();
  return 0;
}
