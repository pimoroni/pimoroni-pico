#pragma once

#include "../common.hpp"

namespace pimoroni {

  class BenchPowerModule : public YukonModule {
  public:
    static const std::string NAME;

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    virtual std::string name() {
      return BenchPowerModule::NAME;
    }

    TYPE_FUNCTION(BenchPowerModule)
  };

}
