#pragma once

#include "../common.hpp"

namespace pimoroni {

  class QuadServoDirectModule : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "Quad Servo Direct";
    }

    virtual std::string instance_name() {
      return QuadServoDirectModule::name();
    }

    INFO_FUNC(QuadServoDirectModule)
  };

}
