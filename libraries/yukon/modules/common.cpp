#include "common.hpp"

namespace pimoroni {
  TCA_IO::TCA_IO(TCA pin, TCAAccessor& accessor) :
    pin(pin),
    accessor(accessor) {
    to_input();
  };

  TCA_IO::TCA_IO(TCA pin, TCAAccessor& accessor, bool out) :
    pin(pin),
    accessor(accessor) {
    direction(out);
  };

  //TCA_IO::~TCA_IO() {
  //  to_input();
  //}

  bool TCA_IO::direction() {
    return accessor.get_slow_config(pin);
  }

  void TCA_IO::direction(bool out) {
    accessor.set_slow_config(pin, out);
  }

  void TCA_IO::to_output(bool val) {
    value(val);
    direction(GPIO_OUT);
  }

  void TCA_IO::to_input() {
    direction(GPIO_IN);
    value(false);
  }

  bool TCA_IO::value() {
    if(direction()) {
        return accessor.get_slow_output(pin);
    }
    else {
        return accessor.get_slow_input(pin);
    }
  }

  void TCA_IO::value(bool val) {
    accessor.set_slow_output(pin, val);
  }



  YukonModule::YukonModule() :
    slot(),
    __accessor(nullptr),
    initialised(false) {

    clear_readings();

    // TODO
    //self.__monitor_action_callback = None
  }

  void YukonModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Record the slot we are in, and the ADC functions to call
    this->slot = slot;
    this->__accessor = &accessor;
    initialised = true;

    // Configure any objects created during initialisation
    configure();
  }

  bool YukonModule::is_initialised() {
    return initialised;
  }

  void YukonModule::deregister() {
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