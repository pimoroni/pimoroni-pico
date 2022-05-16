#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class AnalogMux {
  public:
    AnalogMux(uint addr0_pin, uint addr1_pin = PIN_UNUSED, uint addr2_pin = PIN_UNUSED,
              uint en_pin = PIN_UNUSED, uint muxed_pin = PIN_UNUSED);

    void select(uint8_t address);
    void disable();
    void configure_pulls(uint8_t address, bool pullup, bool pulldown);
    bool read();

  private:
    uint addr0_pin;
    uint addr1_pin;
    uint addr2_pin;
    uint en_pin;
    uint muxed_pin;
    uint max_address;
    uint8_t pull_ups;
    uint8_t pull_downs;
  };

}