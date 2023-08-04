#include "dual_motor.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  const std::string DualMotorModule::NAME = "Dual Motor";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | HIGH  | 1     | 1     | 1     | Dual Motor           |                             |
  // | HIGH  | 0     | 1     | 1     | Dual Motor           |                             |
  bool DualMotorModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_HIGH && slow2 == HIGH && slow3 == HIGH;
  }

  DualMotorModule::DualMotorModule(float frequency) :
    YukonModule(),
    motor_type(DUAL),
    frequency(frequency),
    motors(nullptr) {
  }

  DualMotorModule::~DualMotorModule() {
    delete(motors);
  }

  std::string DualMotorModule::name() {
    return DualMotorModule::NAME;
  }

  void DualMotorModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Configure strip and power pins
    configure();

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void DualMotorModule::configure() {

  }

  void DualMotorModule::enable() {

  }

  void DualMotorModule::disable() {

  }

  bool DualMotorModule::is_enabled() {
    return 0; // TODO
  }

  bool DualMotorModule::decay() {
    return 0; // TODO
  }

  void DualMotorModule::decay(bool val) {

  }

  bool DualMotorModule::toff() {
    return 0; // TODO
  }

  void DualMotorModule::toff(bool val) {

  }

  bool DualMotorModule::read_fault() {
    return __read_adc1() <= FAULT_THRESHOLD;
  }

  float DualMotorModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void DualMotorModule::monitor() {
    bool fault = read_fault();
    if(!fault) {
      throw FaultError(__message_header() + "Fault detected on motor driver! Turning off output\n");
    }

    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    fault_triggered = fault_triggered || fault;
    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;
    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> DualMotorModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("Fault", fault_triggered));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void DualMotorModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
    }
  }

  void DualMotorModule::clear_readings() {
    fault_triggered = true;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
