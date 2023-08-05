#include "io.hpp"

namespace pimoroni {
  IO::IO(uint pin) :
    pin(pin) {
    gpio_init(pin);
  };

  IO::IO(uint pin, bool out, bool pull_up, bool pull_down) :
    pin(pin) {
    gpio_init(pin);
    direction(out, pull_up, pull_down);
  };

  bool IO::direction() {
    return gpio_is_dir_out(pin);
  }

  void IO::direction(bool out, bool pull_up, bool pull_down) {
    gpio_set_dir(pin, out);
    gpio_set_pulls(pin, pull_up, pull_down);
  }

  void IO::to_output(bool val) {
    gpio_set_pulls(pin, false, false);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, val);
  }

  void IO::to_input(bool pull_up, bool pull_down) {
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, pull_up, pull_down);
  }

  bool IO::value() {
    //if(gpio_is_dir_out(pin)) {
    //  // NOTE: Documentation says this is just for debug use...
    //  return gpio_get_out_level(pin);
    //}
    //else {
    return gpio_get(pin);  // Cool! This seems to return the output state as well as the input state
    //}
  }

  void IO::value(bool val) {
    gpio_put(pin, val);
  }
};