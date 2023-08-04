#include "bench_power.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  const std::string BenchPowerModule::NAME = "Bench Power";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 0     | 0     | Bench Power          |                             |
  // | FLOAT | 1     | 0     | 0     | Bench Power          | When V+ is discharging      |
  // FLOAT address included as may not be able to rely on the ADC level being low
  bool BenchPowerModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == LOW && slow3 == LOW;
  }

  BenchPowerModule::BenchPowerModule(bool halt_on_not_pgood) :
    YukonModule(),
    halt_on_not_pgood(halt_on_not_pgood),
    voltage_pwm(nullptr),
    power_en(nullptr),
    power_good(nullptr) {
  }

  BenchPowerModule::~BenchPowerModule() {
    delete(voltage_pwm);
    delete(power_en);
    delete(power_good);
  }

  std::string BenchPowerModule::name() {
    return BenchPowerModule::NAME;
  }

  void BenchPowerModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Configure strip and power pins
    configure();

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void BenchPowerModule::configure() {

  }

  void BenchPowerModule::enable() {

  }

  void BenchPowerModule::disable() {

  }

  bool BenchPowerModule::is_enabled() {
    return 0; // TODO
  }

  void BenchPowerModule::set_pwm(float percent) {

  }

  void BenchPowerModule::set_target_voltage(float voltage) {

  }

  void BenchPowerModule::set_target(float percent) {

  }

  float BenchPowerModule::read_voltage() {
    // return (self.__shared_adc_voltage() * (100 + 22)) / 22
    float value = __read_adc1();
    if(value >= VOLTAGE_MID_MEASURE) {
      return ((value - VOLTAGE_MID_MEASURE) * (VOLTAGE_MAX - VOLTAGE_MID)) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MID_MEASURE) + VOLTAGE_MID;
    }
    else {
      float voltage = ((value - VOLTAGE_MIN_MEASURE) * (VOLTAGE_MID - VOLTAGE_MIN)) / (VOLTAGE_MID_MEASURE - VOLTAGE_MIN_MEASURE) + VOLTAGE_MIN;
      return MAX(voltage, 0.0);
    }
  }

  bool BenchPowerModule::read_power_good() {
    return 0; // TODO
  }

  float BenchPowerModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void BenchPowerModule::monitor() {
    bool pgood = read_power_good();
    if(!pgood) {
      if(halt_on_not_pgood) {
        throw FaultError(__message_header() + "Power is not good! Turning off output\n");
      }
    }

    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    float voltage_out = read_voltage();

    if(last_pgood && !pgood) {
      logging.warn(__message_header() + "Power is not good\n");
    }
    else if(!last_pgood && pgood) {
      logging.warn(__message_header() + "Power is good\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    last_pgood = pgood;
    power_good_throughout = power_good_throughout && pgood;

    max_voltage_out = MAX(voltage_out, max_voltage_out);
    min_voltage_out = MIN(voltage_out, min_voltage_out);
    avg_voltage_out += voltage_out;

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;

    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> BenchPowerModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("PGood", power_good_throughout));
    values.push_back(std::pair("VO_max", max_voltage_out));
    values.push_back(std::pair("VO_min", min_voltage_out));
    values.push_back(std::pair("VO_avg", avg_voltage_out));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void BenchPowerModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
      avg_voltage_out /= count_avg;
    }
  }

  void BenchPowerModule::clear_readings() {
    power_good_throughout = true;
    max_voltage_out = -std::numeric_limits<float>::infinity();
    min_voltage_out = std::numeric_limits<float>::infinity();
    avg_voltage_out = 0;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
