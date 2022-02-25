#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "badger2040.hpp"

using namespace pimoroni;

// this simple example tells you which button was used to wake up
// Badger2040 and then immediately halts again on another button press

Badger2040 badger;

int main() {

  badger.init();

  // find which button was used to wake up
  std::string button = "";
  std::string message = "started up.";
  if(badger.pressed_to_wake(badger.A)) { button += "A"; }
  if(badger.pressed_to_wake(badger.B)) { button += "B"; }
  if(badger.pressed_to_wake(badger.C)) { button += "C"; }
  if(badger.pressed_to_wake(badger.D)) { button += "D"; }
  if(badger.pressed_to_wake(badger.E)) { button += "E"; }

  if(button != "") {
    message = "woken up by button " + button + ".";
  }

  badger.thickness(2);

  badger.pen(15);
  badger.clear();
  badger.pen(0);
  badger.text(message, 10, 20, 0.6f);
  badger.text("(press any button to go to sleep.)", 10, 70, 0.4f);
  badger.update();

  badger.wait_for_press();

  badger.pen(15);
  badger.clear();
  badger.pen(0);
  badger.text("going back to sleep...", 10, 20, 0.6f);
  badger.text("z", 220, 50, 0.6f);
  badger.text("z", 230, 40, 0.8f);
  badger.text("z", 240, 30, 1.0f);
  badger.text("(press any button to wake up.)", 10, 70, 0.4f);
  badger.update();

  badger.halt();

  // proof we halted, the LED will not turn on
  badger.led(255);
}
