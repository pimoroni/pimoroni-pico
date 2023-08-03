#pragma once

#include "modules/common.hpp"
#include "modules/led_strip/led_strip.hpp"
#include "modules/quad_servo/quad_servo_direct.hpp"
#include "modules/quad_servo/quad_servo_reg.hpp"
#include "modules/big_motor/big_motor.hpp"
#include "modules/dual_motor/dual_motor.hpp"
#include "modules/dual_switched/dual_switched.hpp"
#include "modules/bench_power/bench_power.hpp"
//#include "modules/audio_amp/audio_amp.hpp"
#include "modules/proto/proto.hpp"

namespace pimoroni {

  const ModuleType KNOWN_MODULES[] = {
    LEDStripModule::type(),
    QuadServoDirectModule::type(),
    QuadServoRegModule::type(),
    BigMotorModule::type(),
    DualMotorModule::type(),
    DualSwitchedModule::type(),
    BenchPowerModule::type(),
    //AudioAmpModule::type(),
    ProtoPotModule::type(),
  };
}
