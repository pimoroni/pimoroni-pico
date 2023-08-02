#pragma once

#include "../common.hpp"

namespace pimoroni {

  class ProtoPotModule : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "Proto Potentiometer";
    }

    virtual std::string instance_name() {
      return ProtoPotModule::name();
    }

    INFO_FUNC(ProtoPotModule)
  };

  class ProtoPotModule2 : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "Proto Potentiometer 2";
    }

    virtual std::string instance_name() {
      return ProtoPotModule2::name();
    }

    INFO_FUNC(ProtoPotModule2)
  };

}
