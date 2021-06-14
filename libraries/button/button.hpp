#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class Button {
  public:
    Button(uint pin, Polarity polarity=Polarity::ACTIVE_LOW, uint32_t repeat_time=200, uint32_t hold_time=1000) :
    pin(pin), polarity(polarity), repeat_time(repeat_time), hold_time(hold_time) {
      gpio_set_function(pin, GPIO_FUNC_SIO);
      gpio_set_dir(pin, GPIO_IN);
      if (polarity == Polarity::ACTIVE_LOW) {
        gpio_pull_up(pin);
      }
      else {
        gpio_pull_down(pin);
      }
    };
    bool raw();
    bool read();
  private:
    uint pin;
    Polarity polarity;
    uint32_t repeat_time;
    uint32_t hold_time;
    bool pressed = false;
    bool last_state = false;
    uint32_t pressed_time = 0;
    uint32_t last_time = 0;
  };

}