#include "common.hpp"
#include "../logging.hpp"

namespace pimoroni {

  YukonModule::YukonModule() :
    slot(),
    __accessor(nullptr),
    initialised(false) {

    clear_readings();

    // TODO
    //self.__monitor_action_callback = None
  }

  YukonModule::~YukonModule() {
    logging.debug("[YukonModule] Destructor Started\n");
    if(is_initialised()) {
      // Modules make use of IO and TCA_IO classes for pin control then the below changes are redundant
      gpio_deinit(slot.FAST1);
      gpio_deinit(slot.FAST2);
      gpio_deinit(slot.FAST3);
      gpio_deinit(slot.FAST4);
      __accessor->set_slow_config(slot.SLOW1, false);
      __accessor->set_slow_config(slot.SLOW2, false);
      __accessor->set_slow_config(slot.SLOW3, false);

      __accessor->deregister(*this);
    }
    logging.debug("[YukonModule] Destructor Done\n");
  }

  void YukonModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Record the slot we are in, and the ADC functions to call
    this->slot = slot;
    this->__accessor = &accessor;
    initialised = true;

    // Put any objects created during initialisation into a known state
    reset();
  }

  bool YukonModule::is_initialised() {
    return initialised;
  }

  void YukonModule::deregister() {
    logging.debug("[YukonModule] Deregistering\n");
    initialised = false;
    __accessor = nullptr;
  }

  void YukonModule::assign_monitor_action() { //TODO callback_function):
    //if not None and not callable(callback_function):
    //  raise TypeError("callback is not callable or None")

    //self.__monitor_action_callback = callback_function
  }

  std::vector<std::pair<std::string, float>> YukonModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    return values;
  }


  float YukonModule::__read_adc1() {
    CHECK_INITIALISED
    return __accessor->read_slot_adc1(slot);
  }

  float YukonModule::__read_adc2() {
    CHECK_INITIALISED
    return __accessor->read_slot_adc2(slot);
  }

  float YukonModule::__read_adc2_as_temp() {
    CHECK_INITIALISED
    float sense = __accessor->read_slot_adc2(slot);
    float r_thermistor = sense / ((3.3f - sense) / 5100.0f);
    float t_kelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(r_thermistor / RESISTOR_AT_ROOM_TEMP)));
    float t_celsius = t_kelvin - 273.15f;
    // https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
    return t_celsius;
  }

  std::string YukonModule::__message_header() {
    return "[Slot" + std::to_string(slot.ID) + " '" + name() + "'] ";
  }
};