#include "quad_servo_direct.hpp"

namespace pimoroni {

  const std::string QuadServoDirectModule::NAME = "Quad Servo Direct";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 0     | 0     | 0     | Quad Servo Direct    | A1 input near 0V            |
  // | FLOAT | 0     | 0     | 0     | Quad Servo Direct    | A1 input between 0 and 3.3V |
  // | HIGH  | 0     | 0     | 0     | Quad Servo Direct    | A1 input near 3.3V          |
  bool QuadServoDirectModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == LOW && slow2 == LOW && slow3 == LOW;
  }

  QuadServoDirectModule::QuadServoDirectModule() :
    YukonModule(),
    servos(nullptr) {
  }

  QuadServoDirectModule::~QuadServoDirectModule() {
    delete(servos);
  }

  std::string QuadServoDirectModule::name() {
    return QuadServoDirectModule::NAME;
  }

  void QuadServoDirectModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Configure strip and power pins
    configure();

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void QuadServoDirectModule::configure() {

  }

  float QuadServoDirectModule::read_adc1() {
    return __read_adc1();
  }

  float QuadServoDirectModule::read_adc2() {
    return __read_adc2();
  }

}
