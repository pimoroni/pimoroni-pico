#pragma once

#include "../common.hpp"

namespace pimoroni {

  class QuadServoRegModule : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "Quad Servo Regulated";
    }

    virtual std::string instance_name() {
      return QuadServoRegModule::name();
    }

    INFO_FUNC(QuadServoRegModule)
  };

}
