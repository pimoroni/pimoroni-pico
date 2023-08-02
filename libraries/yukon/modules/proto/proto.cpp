#include "proto.hpp"

namespace pimoroni {

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 1     | 0     | Proto Potentiometer  | Pot in low position         |
  // | FLOAT | 1     | 1     | 0     | Proto Potentiometer  | Pot in middle position      |
  // | HIGH  | 1     | 1     | 0     | Proto Potentiometer  | Pot in high position        |
  bool ProtoPotModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == HIGH && slow3 == LOW;
  }

  bool ProtoPotModule2::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == HIGH && slow3 == LOW;
  }

}
