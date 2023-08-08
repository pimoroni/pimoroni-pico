#include "tca_io.hpp"

namespace pimoroni {
  TCA_IO::TCA_IO(TCA pin, TCAAccessor& accessor) :
    pin(pin),
    accessor(accessor) {
    to_input();
  };

  TCA_IO::TCA_IO(TCA pin, TCAAccessor& accessor, bool out) :
    pin(pin),
    accessor(accessor) {
    direction(out);
  };

  TCA_IO::~TCA_IO() {
    to_input();
  }

  bool TCA_IO::direction() {
    return accessor.get_slow_config(pin);
  }

  void TCA_IO::direction(bool out) {
    accessor.set_slow_config(pin, out);
  }

  void TCA_IO::to_output(bool val) {
    value(val);
    direction(GPIO_OUT);
  }

  void TCA_IO::to_input() {
    direction(GPIO_IN);
    value(false);
  }

  bool TCA_IO::value() {
    if(direction()) {
      return accessor.get_slow_output(pin);
    }
    else {
      return accessor.get_slow_input(pin);
    }
  }

  void TCA_IO::value(bool val) {
    accessor.set_slow_output(pin, val);
  }

};
