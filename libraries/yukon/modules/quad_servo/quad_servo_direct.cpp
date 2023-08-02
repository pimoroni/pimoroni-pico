#include "quad_servo_direct.hpp"

namespace pimoroni {

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 0     | 0     | 0     | Quad Servo Direct    | A1 input near 0V            |
  // | FLOAT | 0     | 0     | 0     | Quad Servo Direct    | A1 input between 0 and 3.3V |
  // | HIGH  | 0     | 0     | 0     | Quad Servo Direct    | A1 input near 3.3V          |
  bool QuadServoDirectModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == LOW && slow2 == LOW && slow3 == LOW;
  }

}
