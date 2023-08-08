#pragma once

#include "pico/stdlib.h"
#include "tca_io.hpp"

#include <math.h>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <vector>

namespace pimoroni {

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

  enum IOState {
    LOW = false,
    HIGH = true
  };

  class YukonModule;  // Forward declaration

  class SlotAccessor : public TCAAccessor {
  public:
    virtual float read_slot_adc1(SLOT slot) = 0;
    virtual float read_slot_adc2(SLOT slot) = 0;

    virtual void deregister(YukonModule& module) = 0;
  };

  typedef bool (&func_is_module)(uint, bool, bool, bool);

  struct ModuleType {
    const std::type_index TYPE;   // The typeid of the module class, used for checking if a detected module is a registered module
    const std::string& NAME;      // The name of the module class to display to the user
    func_is_module is_module;     // A reference to the function to call to detect a module of this type
  };

  class YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    // Temperature calculation constants
    static constexpr float ROOM_TEMP = 273.15f + 25.0f;
    static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
    static constexpr float BETA = 3435;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    SLOT slot;
  private:
    SlotAccessor* __accessor;
    bool initialised;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    YukonModule();
    virtual ~YukonModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    // Required name accessor
    virtual std::string name() = 0;

    // Initialise the module once it has been assigned a slot
    // Any overriding function must call this parent function at its end
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);

    // Override this to put the module into a known state
    virtual void reset() {}

    //--------------------------------------------------
    bool is_initialised();
    void deregister();
    void assign_monitor_action(); //TODO callback_function):

    //--------------------------------------------------
    // Override this to perform any monitoring operations specific to the module
    virtual void monitor() {}

    // Override this to access the readings obtained from monitoring
    virtual std::vector<std::pair<std::string, float>> get_readings();

    // Override this to calculate averages, or do other post-processing on readings after monitoring
    virtual void process_readings() {}

    // Override this to clear any readings that may accumulate, such as min, max, or average
    virtual void clear_readings() {}

    //--------------------------------------------------
    // Useful functions for derived classes
  protected:
    float __read_adc1();
    float __read_adc2();
    float __read_adc2_as_temp();
    std::string __message_header();
  };

#define TYPE_FUNCTION(module_class) \
  static ModuleType type() { \
    return { typeid(module_class), module_class::NAME, module_class::is_module }; \
  }

#define CHECK_INITIALISED \
  if(!is_initialised()) { \
    throw std::runtime_error("Module not initialised\n"); \
  }

}
