#pragma once

#include "../common.hpp"

namespace pimoroni {

  class AudioAmpModule : public YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    static std::string name() {
      return "Audio Amp";
    }

    virtual std::string instance_name() {
      return AudioAmpModule::name();
    }

    INFO_FUNC(AudioAmpModule)
  };

}
