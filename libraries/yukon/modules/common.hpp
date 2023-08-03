#pragma once

#include "pico/stdlib.h"
#include <typeindex>
#include <string>
#include <vector>
#include <math.h>
#include <stdexcept>
namespace pimoroni {

  struct TCA {
    uint CHIP;
    uint GPIO;
  };

  struct SLOT {
    uint ID;
    uint FAST1;
    uint FAST2;
    uint FAST3;
    uint FAST4;
    TCA SLOW1;
    TCA SLOW2;
    TCA SLOW3;
    uint ADC1_ADDR;
    uint ADC2_TEMP_ADDR;

    // Needed for use with std::map
    bool operator<(const SLOT& o) const {
      return (ID < o.ID);
    }
  };

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

  class SlotAccessor {
  public:
    virtual bool get_slow_input(TCA gpio) = 0;
    virtual bool get_slow_output(TCA gpio) = 0;
    virtual bool get_slow_config(TCA gpio) = 0;
    virtual bool get_slow_polarity(TCA gpio) = 0;

    virtual void set_slow_output(TCA gpio, bool value) = 0;
    virtual void set_slow_config(TCA gpio, bool output) = 0;
    virtual void set_slow_polarity(TCA gpio, bool polarity) = 0;

    virtual float read_slot_adc1(SLOT slot) = 0;
    virtual float read_slot_adc2(SLOT slot) = 0;
  };

  class YukonModule {
  public:
    static constexpr float ROOM_TEMP = 273.15f + 25.0f;
    static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
    static constexpr float BETA = 3435;

    virtual std::string name() = 0;

    SLOT slot;
    SlotAccessor* __accessor;
    bool initialised;

    YukonModule() :
      slot(),
      __accessor(nullptr),
      initialised(false) {

      clear_readings();

      // TODO
      //self.__monitor_action_callback = None
    }

    virtual void initialise(const SLOT& slot, SlotAccessor& accessor) {
      // Record the slot we are in, and the ADC functions to call
      this->slot = slot;
      this->__accessor = &accessor;
      initialised = true;
    }

    bool is_initialised() {
      return initialised;
    }

    void deregister() {
      initialised = false;
      __accessor = nullptr;
    }

    virtual void configure() {
      // Function for (re)configuring pins etc to their default states needed by the module
    }

    float __read_adc1() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      return __accessor->read_slot_adc1(slot);
    }

    float __read_adc2() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      return __accessor->read_slot_adc2(slot);
    }

    float __read_adc2_as_temp() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      float sense = __accessor->read_slot_adc2(slot);
      float r_thermistor = sense / ((3.3f - sense) / 5100.0f);
      float t_kelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(r_thermistor / RESISTOR_AT_ROOM_TEMP)));
      float t_celsius = t_kelvin - 273.15f;
      // https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
      return t_celsius;
    }

    void assign_monitor_action() { //TODO callback_function):
      //if not None and not callable(callback_function):
      //  raise TypeError("callback is not callable or None")

      //self.__monitor_action_callback = callback_function
    }

    virtual void monitor() {
    }

    virtual std::vector<std::pair<std::string, float>> get_readings() {
      std::vector<std::pair<std::string, float>> values;
      return values;
    }

    virtual void process_readings() {
      // Use this to calculate averages, or do other post-processing on readings after monitor
    }

    virtual void clear_readings() {
      // Clear any readings that may accumulate, such as min, max, or average
    }

    std::string __message_header() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      return "[Slot" + std::to_string(slot.ID) + " '" + name() + "'] ";
    }

  };

  typedef bool (&func_is_module)(uint, bool, bool, bool);

  struct ModuleType {
    const std::type_index TYPE;   // The typeid of the module class, used for checking if a detected module is a registered module
    const std::string& NAME;      // The name of the module class to display to the user
    func_is_module is_module;     // A reference to the function to call to detect a module of this type
  };

#define TYPE_FUNCTION(module_class) \
  static ModuleType type() { \
    return { typeid(module_class), module_class::NAME, module_class::is_module }; \
  }

}
