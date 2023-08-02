#include "quad_servo_reg.hpp"

namespace pimoroni {

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 0     | 1     | 0     | Quad Servo Regulated |                             |
  bool QuadServoRegModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_FLOAT && slow1 == LOW && slow2 == HIGH && slow3 == LOW;
  }

}
