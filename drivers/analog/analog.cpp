#include "analog.hpp"

namespace pimoroni {
    uint16_t Analog::read_raw() {
      adc_select_input(pin - 26);
      return adc_read();
    }

    float Analog::read_voltage() {
      adc_select_input(pin - 26);
      float voltage = ((((float)adc_read() * 3.3f) / (1 << 12)) + offset) / amplifier_gain;
      return MAX(voltage, 0.0f);
    }

    float Analog::read_current() {
      if(resistor > 0.0f)
        return read_voltage() / resistor;
      else
        return read_voltage();
    }
};