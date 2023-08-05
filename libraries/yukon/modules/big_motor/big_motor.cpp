#include "big_motor.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  const std::string BigMotorModule::NAME = "Big Motor";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 0     | 0     | 1     | Big Motor            | Not in fault                |
  // | LOW   | 0     | 1     | 1     | Big Motor            | In fault                    |
  bool BigMotorModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_LOW && slow1 == LOW && slow3 == HIGH;
  }

  BigMotorModule::BigMotorModule(float frequency) :
    YukonModule(),
    frequency(frequency),
    motor(nullptr),
    encoder(nullptr),
    motor_en(nullptr),
    motor_nfault(nullptr) {
  }

  BigMotorModule::~BigMotorModule() {
    delete(motor);
    delete(encoder);
    delete(motor_en);
    delete(motor_nfault);
  }

  std::string BigMotorModule::name() {
    return BigMotorModule::NAME;
  }

  void BigMotorModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create motor object
    motor = new MotorCluster(pio0, 0, slot.FAST3, NUM_MOTORS);

    // Create motor control pin objects
    motor_en = new TCA_IO(slot.SLOW3, accessor);
    motor_nfault = new TCA_IO(slot.SLOW2, accessor);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void BigMotorModule::configure() {
    CHECK_INITIALISED
    motor->disable_all();
    motor->decay_mode(0, SLOW_DECAY);

    motor_nfault ->to_input();
    motor_en->to_output(false);
  }

  void BigMotorModule::enable() {
    CHECK_INITIALISED
    motor_en->value(true);
  }

  void BigMotorModule::disable() {
    CHECK_INITIALISED
    motor_en->value(true);
  }

  bool BigMotorModule::is_enabled() {
    CHECK_INITIALISED
    return motor_en->value();
  }

  bool BigMotorModule::read_fault() {
    CHECK_INITIALISED
    return !motor_nfault->value();
  }

  bool BigMotorModule::read_current() {
    // This needs more validation
    return (fabsf(__read_adc1() - (3.3f / 2.0f))) / (SHUNT_RESISTOR * GAIN);
  }

  float BigMotorModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void BigMotorModule::monitor() {
    bool fault = read_fault();
    if(!fault) {
      throw FaultError(__message_header() + "Fault detected on motor driver! Turning off output\n");
    }

    float current = read_current();
    if(current > CURRENT_THRESHOLD) {
      throw OverCurrentError(__message_header() + "Current of " + std::to_string(current) + "A exceeded the user set level of " + std::to_string(CURRENT_THRESHOLD) + "A! Turning off output\n");
    }

    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    fault_triggered = fault_triggered || fault;

    max_current = MAX(current, max_current);
    min_current = MIN(current, min_current);
    avg_current += current;

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;

    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> BigMotorModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("Fault", fault_triggered));
    values.push_back(std::pair("C_max", max_current));
    values.push_back(std::pair("C_min", min_current));
    values.push_back(std::pair("C_avg", avg_current));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void BigMotorModule::process_readings() {
    if(count_avg > 0) {
      avg_current /= count_avg;
      avg_temperature /= count_avg;
    }
  }

  void BigMotorModule::clear_readings() {
    fault_triggered = true;
    max_current = -std::numeric_limits<float>::infinity();
    min_current = std::numeric_limits<float>::infinity();
    avg_current = 0;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
