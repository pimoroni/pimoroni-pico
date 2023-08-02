#pragma once

#include "pico/stdlib.h"
#include <typeindex>
#include <string>

namespace pimoroni {

  enum ADC {
    ADC_LOW = 0,
    ADC_HIGH = 1,
    ADC_FLOAT = 2,
    ADC_ANY = 3
  };

  enum IO {
    LOW = false,
    HIGH = true
  };

  class YukonModule {
    public:
      //static const std::string NAME = "Unnamed";

      static constexpr float ROOM_TEMP = 273.15f + 25.0f;
      static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
      static constexpr float BETA = 3435;

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
      return false;
    }

    virtual std::string instance_name() = 0;
  };

  typedef bool (*module_callback)(uint, bool, bool, bool) ;

  struct ModuleInfo {
    std::type_index type;
    std::string name;
    module_callback is_module;
  };

#define INFO_FUNC(module_name) \
    static ModuleInfo info() { \
      return { typeid(module_name), module_name::name(), &module_name::is_module }; \
      }

}
