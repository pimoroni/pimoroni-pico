#include "analog.hpp"

namespace pimoroni {
    uint16_t Analog::read_raw() {
      return adc_read();
    }

    float Analog::read_voltage() {
      return ((float)adc_read() * 3.3f) / (1 << 12) / amplifier_gain;
    }

    float Analog::read_current() {
      if(resistor > 0.0f)
        return read_voltage() / resistor;
      else
        return read_voltage();
    }
};