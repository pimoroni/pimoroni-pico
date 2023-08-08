#include "quad_servo_direct.hpp"
#include "../../logging.hpp"

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
    logging.debug("[QuadServoDirectModule] Destructor Started\n");
    delete(servos);
    logging.debug("[QuadServoDirectModule] Destructor Done\n");
  }

  std::string QuadServoDirectModule::name() {
    return QuadServoDirectModule::NAME;
  }

  void QuadServoDirectModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create servo cluster object
    servos = new ServoCluster(pio0, 0, slot.FAST1, NUM_SERVOS);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void QuadServoDirectModule::reset() {
    logging.debug("[QuadServoDirectModule] Resetting\n");
    CHECK_INITIALISED
    servos->disable_all();
  }

  float QuadServoDirectModule::read_adc1() {
    return __read_adc1();
  }

  float QuadServoDirectModule::read_adc2() {
    return __read_adc2();
  }

}
