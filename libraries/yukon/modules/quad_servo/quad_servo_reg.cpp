#include "quad_servo_reg.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  const std::string QuadServoRegModule::NAME = "Quad Servo Regulated";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 0     | 1     | 0     | Quad Servo Regulated |                             |
  bool QuadServoRegModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_FLOAT && slow1 == LOW && slow2 == HIGH && slow3 == LOW;
  }

  QuadServoRegModule::QuadServoRegModule(bool halt_on_not_pgood) :
    YukonModule(),
    halt_on_not_pgood(halt_on_not_pgood),
    servos(nullptr),
    power_en(nullptr),
    power_good(nullptr) {
  }

  QuadServoRegModule::~QuadServoRegModule() {
    delete(servos);
    delete(power_en);
    delete(power_good);
  }

  std::string QuadServoRegModule::name() {
    return QuadServoRegModule::NAME;
  }

  void QuadServoRegModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create servo cluster object
    servos = new ServoCluster(pio0, 0, slot.FAST1, NUM_SERVOS);

    // Create the power control pin objects
    power_en = new TCA_IO(slot.SLOW1, accessor);
    power_good = new TCA_IO(slot.SLOW2, accessor);

    // Configure strip and power pins
    configure();

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void QuadServoRegModule::configure() {
    servos->disable_all();

    power_en->to_output(false);
    power_good->to_input();
  }

  void QuadServoRegModule::enable() {
    CHECK_INITIALISED
    power_en->value(true);
  }

  void QuadServoRegModule::disable() {
    CHECK_INITIALISED
    power_en->value(false);
  }

  bool QuadServoRegModule::is_enabled() {
    CHECK_INITIALISED
    return power_en->value();
  }

  bool QuadServoRegModule::read_power_good() {
    CHECK_INITIALISED
    return power_good->value();
  }

  float QuadServoRegModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void QuadServoRegModule::monitor() {
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

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;
    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> QuadServoRegModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("PGood", power_good_throughout));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void QuadServoRegModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
    }
  }

  void QuadServoRegModule::clear_readings() {
    power_good_throughout = true;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
