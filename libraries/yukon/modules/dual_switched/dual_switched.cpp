#include "dual_switched.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  const std::string DualSwitchedModule::NAME = "Dual Switched Output";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 1     | 0     | 1     | Dual Switched Output |                             |
  bool DualSwitchedModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_FLOAT && slow1 == HIGH && slow2 == LOW && slow3 == HIGH;
  }

  DualSwitchedModule::DualSwitchedModule(bool halt_on_not_pgood) :
    YukonModule(),
    halt_on_not_pgood(halt_on_not_pgood),
    last_pgood1(false),
    last_pgood2(false) {
    //sw_output(nullptr, nullptr), TODO
    //sw_enable(nullptr, nullptr) {
  }

  DualSwitchedModule::~DualSwitchedModule() {
    delete(sw_output[0]);
    delete(sw_output[1]);
    delete(sw_enable[0]);
    delete(sw_enable[1]);
    delete(power_good[0]);
    delete(power_good[1]);
  }

  std::string DualSwitchedModule::name() {
    return DualSwitchedModule::NAME;
  }

  void DualSwitchedModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create the switch and power control pin objects
    sw_output[0] = new IO(slot.FAST1);
    sw_output[1] = new IO(slot.FAST3);
    sw_enable[0] = new IO(slot.FAST1);
    sw_enable[1] = new IO(slot.FAST3);
    power_good[0] = new TCA_IO(slot.SLOW1, accessor);
    power_good[1] = new TCA_IO(slot.SLOW3, accessor);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void DualSwitchedModule::configure() {
    CHECK_INITIALISED
    sw_output[0]->to_output(false);
    sw_output[1]->to_output(false);

    sw_enable[0]->to_output(false);
    sw_enable[1]->to_output(false);

    power_good[0]->to_input();
    power_good[1]->to_input();
  }

  void DualSwitchedModule::enable(uint output) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    sw_enable[output - 1]->value(true);
  }

  void DualSwitchedModule::disable(uint output) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    sw_enable[output - 1]->value(false);
  }

  bool DualSwitchedModule::is_enabled(uint output) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    return sw_enable[output - 1]->value();
  }

  void DualSwitchedModule::output(uint output, bool val) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    sw_output[output - 1]->value(val);
  }

  bool DualSwitchedModule:: read_output(uint output) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    return sw_output[output - 1]->value();
  }

  bool DualSwitchedModule::read_power_good(uint output) {
    CHECK_INITIALISED
    if(output < 1 || output > NUM_SWITCHES) {
      throw std::runtime_error("switch index out of range. Expected 1 to 2\n");
    }

    return power_good[output - 1]->value();
  }

  float DualSwitchedModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void DualSwitchedModule::monitor() {
    bool pgood1 = read_power_good(1);
    if(!pgood1) {
      if(halt_on_not_pgood) {
        throw FaultError(__message_header() + "Power1 is not good! Turning off output\n");
      }
    }

    bool pgood2 = read_power_good(2);
    if(!pgood2) {
      if(halt_on_not_pgood) {
        throw FaultError(__message_header() + "Power2 is not good! Turning off output\n");
      }
    }

    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    if(last_pgood1 && !pgood1) {
      logging.warn(__message_header() + "Power1 is not good\n");
    }
    else if(!last_pgood1 && pgood1) {
      logging.warn(__message_header() + "Power1 is good\n");
    }

    if(last_pgood2 && !pgood2) {
      logging.warn(__message_header() + "Power2 is not good\n");
    }
    else if(!last_pgood2 && pgood2) {
      logging.warn(__message_header() + "Power2 is good\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    last_pgood1 = pgood1;
    last_pgood2 = pgood2;
    power_good_throughout1 = power_good_throughout1 && pgood1;
    power_good_throughout2 = power_good_throughout2 && pgood2;

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;
    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> DualSwitchedModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("PGood1", power_good_throughout1));
    values.push_back(std::pair("PGood2", power_good_throughout2));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void DualSwitchedModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
    }
  }

  void DualSwitchedModule::clear_readings() {
    power_good_throughout1 = true;
    power_good_throughout2 = true;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
