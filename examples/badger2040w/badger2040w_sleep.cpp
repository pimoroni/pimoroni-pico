#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "badger2040w.hpp"

using namespace pimoroni;

// this simple example tells you which button was used to wake up
// Badger2040W and then immediately halts again on another button press

Badger2040W badger;

int main() {

  badger.init();
  badger.uc8151->set_update_speed(2);

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

  badger.graphics->set_thickness(2);

  badger.graphics->set_pen(15);
  badger.graphics->clear();
  badger.graphics->set_pen(0);
  badger.graphics->text(message, Point(10, 20), 286, 2);
  badger.graphics->text("(press any button to go to sleep.)", Point(10, 100), 200, 2);
  badger.update();

  badger.led(255);
  badger.wait_for_press();

  badger.graphics->set_pen(15);
  badger.graphics->clear();
  badger.graphics->set_pen(0);
  badger.graphics->text("going back to sleep...", Point(10, 20), 200, 3);
  badger.graphics->text("z", Point(220, 50), 200, 3);
  badger.graphics->text("z", Point(235, 30), 200, 4);
  badger.graphics->text("z", Point(255, 5), 200, 5);
  badger.graphics->text("(press any button to wake up.)", Point(10, 100), 200, 2);
  badger.update();

  while (badger.uc8151->is_busy()) {
    sleep_ms(10);
  }
  badger.led(0);

  badger.halt();

  // proof we halted, the LED will not turn on
  badger.led(255);
}
