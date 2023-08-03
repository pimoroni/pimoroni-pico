#include "led_strip.hpp"

namespace pimoroni {

  const std::string LEDStripModule::NAME = "LED Strip";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 1     | 1     | LED Strip            |                             |
  bool LEDStripModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_LOW && slow1 == HIGH && slow2 == HIGH && slow3 == HIGH;
  }

}
