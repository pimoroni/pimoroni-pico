#include "dual_switched.hpp"

namespace pimoroni {

  const std::string DualSwitchedModule::NAME = "Dual Switched Output";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 1     | 0     | 1     | Dual Switched Output |                             |
  bool DualSwitchedModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_FLOAT && slow1 == HIGH && slow2 == LOW && slow3 == HIGH;
  }

}
