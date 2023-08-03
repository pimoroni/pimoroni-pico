#pragma once

#include "../common.hpp"

namespace pimoroni {

  class ProtoPotModule : public YukonModule {
  public:
    static const std::string NAME;

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    virtual std::string name() {
      return ProtoPotModule::NAME;
    }

    TYPE_FUNCTION(ProtoPotModule)
  };

  class ProtoPotModule2 : public YukonModule {
  public:
    static const std::string NAME;

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    virtual std::string name() {
      return ProtoPotModule2::NAME;
    }

    TYPE_FUNCTION(ProtoPotModule2)
  };

}
