#include "dual_motor.hpp"

namespace pimoroni {

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | HIGH  | 1     | 1     | 1     | Dual Motor           |                             |
  // | HIGH  | 0     | 1     | 1     | Dual Motor           |                             |
  bool DualMotorModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_HIGH && slow2 == HIGH && slow3 == HIGH;
  }

}
