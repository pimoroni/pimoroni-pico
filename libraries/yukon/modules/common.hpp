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
    static constexpr float ROOM_TEMP = 273.15f + 25.0f;
    static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
    static constexpr float BETA = 3435;

    virtual std::string name() = 0;
  };

  typedef bool (&func_is_module)(uint, bool, bool, bool);

  struct ModuleType {
    const std::type_index TYPE;
    const std::string &NAME;
    func_is_module is_module;
  };

#define TYPE_FUNCTION(module_class) \
  static ModuleType type() { \
    return { typeid(module_class), module_class::NAME, module_class::is_module }; \
  }

}
