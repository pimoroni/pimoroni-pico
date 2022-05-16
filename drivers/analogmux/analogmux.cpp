#include "analogmux.hpp"
#include <cstdio>

namespace pimoroni {
  AnalogMux::AnalogMux(uint addr0_pin, uint addr1_pin, uint addr2_pin,
                       uint en_pin, uint muxed_pin)
  : addr0_pin(addr0_pin), addr1_pin(addr1_pin), addr2_pin(addr2_pin)
  , en_pin(en_pin), muxed_pin(muxed_pin), pull_ups(0), pull_downs(0) {

    gpio_init(addr0_pin);
    gpio_set_dir(addr0_pin, GPIO_OUT);
    max_address = 0b001;

    if(addr1_pin != PIN_UNUSED) {
      gpio_init(addr1_pin);
      gpio_set_dir(addr1_pin, GPIO_OUT);
      max_address = 0b011;

      if(addr2_pin != PIN_UNUSED) {
        gpio_init(addr2_pin);
        gpio_set_dir(addr2_pin, GPIO_OUT);
        max_address = 0b111;
      }
    }

    if(en_pin != PIN_UNUSED) {
      gpio_init(en_pin);
      gpio_set_dir(en_pin, GPIO_OUT);
    }

    if(muxed_pin != PIN_UNUSED) {
      gpio_set_input_enabled(muxed_pin, true);
    }
  }

  void AnalogMux::select(uint8_t address) {
    if(address <= max_address) {
      bool to_pull_up = (pull_ups & (1u << address));
      bool to_pull_down = (pull_downs & (1u << address));
      if((muxed_pin != PIN_UNUSED) && !(to_pull_up || to_pull_down)) {
        gpio_disable_pulls(muxed_pin);
      }

      gpio_put(addr0_pin, address & 0b001);

      if(addr1_pin != PIN_UNUSED) {
        gpio_put(addr1_pin, address & 0b010);
      }

      if(addr2_pin != PIN_UNUSED) {
        gpio_put(addr2_pin, address & 0b100);
      }

      if(en_pin != PIN_UNUSED) {
        gpio_put(en_pin, true);
      }

      if((muxed_pin != PIN_UNUSED) && (to_pull_up || to_pull_down)) {
        gpio_set_pulls(muxed_pin, to_pull_up, to_pull_down);
      }
      sleep_us(10); // Add a delay to let the pins settle before taking a reading
    }
  }

  void AnalogMux::disable() {
    if(en_pin != PIN_UNUSED) {
      gpio_put(en_pin, false);
    }
  }

  void AnalogMux::configure_pulls(uint8_t address, bool pullup, bool pulldown) {
    if(address <= max_address) {
      if(pullup)
        pull_ups |= (1u << address);
      else
        pull_ups &= ~(1u << address);

      if(pulldown)
        pull_downs |= (1u << address);
      else
        pull_downs &= ~(1u << address);
    }
  }

  bool AnalogMux::read() {
    if(muxed_pin != PIN_UNUSED) {
      return gpio_get(muxed_pin);
    }
    return false;
  }
}