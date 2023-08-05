#include "pico/stdlib.h"

#include "yukon.hpp"
using namespace pimoroni;
/*
Demonstrates how to create a Servo object and control it.
*/

Yukon y = Yukon();

#define CHAR_CTRL_A (1)
#define CHAR_CTRL_B (2)
#define CHAR_CTRL_C (3)
#define CHAR_CTRL_D (4)

int last_char = PICO_ERROR_TIMEOUT;

bool check_for_ctrla() {
  int c = getchar_timeout_us(0);
  if(c != PICO_ERROR_TIMEOUT)
    printf(std::to_string(c).c_str());
  return c == CHAR_CTRL_A;
}

bool check_for_ctrlb() {
  int c = getchar_timeout_us(0);
  if(c != PICO_ERROR_TIMEOUT)
    printf(std::to_string(c).c_str());
  return c == CHAR_CTRL_B;
}

bool check_for_ctrlc() {
  int c = getchar_timeout_us(0);
  if(c == CHAR_CTRL_C) {
    printf("CTRL-C\n");
    return true;
  }
  return false;
}

bool check_for_ctrld() {
  int c = getchar_timeout_us(0);
  if(c != PICO_ERROR_TIMEOUT)
    printf(std::to_string(c).c_str());
  return c == CHAR_CTRL_D;
}

int main() {
  stdio_init_all();
  y.change_logging(3);

  try {
    // Initialise the servo
    y.init();

    while (!stdio_usb_connected()) {
      sleep_ms(100);
    }
    while(!check_for_ctrlc());
    while(!check_for_ctrlc());
    printf("stdio_usb_connected()\n");

    LEDStripModule strip(LEDStripModule::DOTSTAR, 60);
    //LEDStripModule strip(LEDStripModule::NEOPIXEL, 60);
    //y.find_slots_with_module(LEDStripModule::info());
    //y.find_slots_with_module(DualSwitchedModule::info());
    //y.find_slots_with_module(BenchPowerModule::info());
    y.register_with_slot(strip, 3);
    y.initialise_modules();

    //y.detect_module(Yukon::SLOT1);
    //y.detect_module(Yukon::SLOT2);
    //y.detect_module(Yukon::SLOT3);
    //y.detect_module(Yukon::SLOT4);
    //y.detect_module(Yukon::SLOT5);
    //y.detect_module(Yukon::SLOT6);

    y.enable_main_output();
    strip.enable();

    while(!y.is_boot_pressed()) {
      y.monitored_sleep_ms(100);

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

  y.reset();
  return 0;
}
