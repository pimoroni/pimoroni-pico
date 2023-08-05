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
    // Create the voltage pwm object
    voltage_pwm = new PWMCluster(pio0, 0, slot.FAST2, 1, true);

    // Create the power control pin objects
    power_en = new IO(slot.FAST1);
    power_good = new TCA_IO(slot.SLOW1, accessor);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void BenchPowerModule::configure() {
    CHECK_INITIALISED
    // Calculate a suitable pwm wrap period for this frequency
    uint32_t period; uint32_t div256;
    if(pimoroni::PWMCluster::calculate_pwm_factors(250000, period, div256)) {
      pwm_period = period;

      // Update the pwm before setting the new wrap
      voltage_pwm->set_chan_level(0, 0, false);
      voltage_pwm->set_chan_offset(0, 0, false);

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      voltage_pwm->set_wrap(pwm_period, true); // NOTE Minus 1 not needed here. Maybe should change Wrap behaviour so it is needed, for consistency with hardware pwm?

      // Apply the new divider
      // This is done after loading new PWM values to avoid a lockup condition
      uint8_t div = div256 >> 8;
      uint8_t mod = div256 % 256;
      voltage_pwm->set_clkdiv_int_frac(div, mod);
    }

    power_en->to_output(false);
    power_good->to_input();
  }

  void BenchPowerModule::enable() {
    CHECK_INITIALISED
    power_en->value(true);
  }

  void BenchPowerModule::disable() {
    CHECK_INITIALISED
    power_en->value(false);
  }

  bool BenchPowerModule::is_enabled() {
    CHECK_INITIALISED
    return power_en->value();
  }

  void BenchPowerModule::set_pwm(float percent) {
    voltage_pwm->set_chan_level(0, (uint32_t)(percent * (float)pwm_period));
  }

  void BenchPowerModule::set_target_voltage(float voltage) {
    CHECK_INITIALISED
    float percent;
    if(voltage >= VOLTAGE_MID) {
      percent = (voltage - VOLTAGE_MID) * 0.5f / (VOLTAGE_MAX - VOLTAGE_MID) + 0.5f;
      percent = MIN(percent, 1.0f);
    }
    else {
      percent = (voltage - VOLTAGE_MIN) * 0.5f / (VOLTAGE_MID - VOLTAGE_MIN);
      percent = MAX(percent, 0.0);
    }
    set_target(percent);
  }

  void BenchPowerModule::set_target(float percent) {
    CHECK_INITIALISED
    if(percent < 0.0f || percent > 1.0f) {
      throw std::invalid_argument("percent out of range. Expected 0.0 to 1.0\n");
    }

    set_pwm((percent * (PWM_MAX - PWM_MIN)) + PWM_MIN);
  }

  float BenchPowerModule::read_voltage() {
    // return (self.__shared_adc_voltage() * (100 + 22)) / 22
    float value = __read_adc1();
    float voltage;
    if(value >= VOLTAGE_MID_MEASURE) {
      voltage = ((value - VOLTAGE_MID_MEASURE) * (VOLTAGE_MAX - VOLTAGE_MID)) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MID_MEASURE) + VOLTAGE_MID;
    }
    else {
      voltage = ((value - VOLTAGE_MIN_MEASURE) * (VOLTAGE_MID - VOLTAGE_MIN)) / (VOLTAGE_MID_MEASURE - VOLTAGE_MIN_MEASURE) + VOLTAGE_MIN;
      voltage = MAX(voltage, 0.0);
    }
    return voltage;
  }

  bool BenchPowerModule::read_power_good() {
    return power_good->value();
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
