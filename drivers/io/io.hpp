#pragma once

#include "pico/stdlib.h"

namespace pimoroni {

  class IO {
  public:
    IO(uint pin);
    IO(uint pin, bool out, bool pull_up = false, bool pull_down = false);
    bool direction();
    void direction(bool out, bool pull_up = false, bool pull_down = false);
    void to_output(bool val);
    void to_input(bool pull_up = false, bool pull_down = false);
    bool value();
    void value(bool val);
  private:
    uint pin;
  };

}