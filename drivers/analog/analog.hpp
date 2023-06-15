#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class Analog {
  public:
    Analog(uint pin, float amplifier_gain = 1.0f, float resistor = 0.0f, float offset = 0.0f) :
    pin(pin), amplifier_gain(amplifier_gain), resistor(resistor), offset(offset) {
      if (!(adc_hw->cs & ADC_CS_EN_BITS)) adc_init();

      //Make sure GPIO is high-impedance, no pullups etc
      adc_gpio_init(pin);

      //Select ADC input 0 (GPIO26)
      adc_select_input(pin - 26);
    };
    uint16_t read_raw();
    float read_voltage();
    float read_current();
  private:
    uint pin;
    float amplifier_gain;
    float resistor;
    float offset;
  };

}