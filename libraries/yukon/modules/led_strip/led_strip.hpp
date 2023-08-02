#pragma once

#include "../common.hpp"

namespace pimoroni {

  class LEDStripModule : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "LED Strip";
    }

    virtual std::string instance_name() {
      return LEDStripModule::name() + " (NeoPixel)";
    }

    INFO_FUNC(LEDStripModule)
  };

}
