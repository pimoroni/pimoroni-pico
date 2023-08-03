#include "big_motor.hpp"

namespace pimoroni {

  const std::string BigMotorModule::NAME = "Big Motor";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 0     | 0     | 1     | Big Motor            | Not in fault                |
  // | LOW   | 0     | 1     | 1     | Big Motor            | In fault                    |
  bool BigMotorModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_LOW && slow1 == LOW && slow3 == HIGH;
  }

}
