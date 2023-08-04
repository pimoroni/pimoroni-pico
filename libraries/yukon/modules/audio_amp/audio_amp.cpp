#include "audio_amp.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"

namespace pimoroni {

  static const std::string NAME = "Audio Amp";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 0     | 1     | 1     | [Proposed] Audio Amp |                             |
  bool AudioAmpModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    // return adc_level == ADC_FLOAT && slow1 == LOW && slow2 == HIGH && slow3 == HIGH;
    return adc_level == ADC_FLOAT && slow1 == HIGH && slow2 == HIGH && slow3 == HIGH;
  }

  AudioAmpModule::AudioAmpModule() :
    YukonModule() {
  }

  AudioAmpModule::~AudioAmpModule() {
  }

  std::string AudioAmpModule::name() {
    return AudioAmpModule::NAME;
  }

  void AudioAmpModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Configure strip and power pins
    configure();

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void AudioAmpModule::configure() {

  }

  void AudioAmpModule::enable() {

  }

  void AudioAmpModule::disable() {

  }

  bool AudioAmpModule::is_enabled() {
    return 0; // TODO
  }

  void AudioAmpModule::exit_soft_shutdown() {

  }

  void AudioAmpModule::set_volume(float volume) {

  }

  float AudioAmpModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void AudioAmpModule::monitor() {
    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;
    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> AudioAmpModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void AudioAmpModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
    }
  }

  void AudioAmpModule::clear_readings() {
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

  void AudioAmpModule::start_i2c() {

  }

  void AudioAmpModule::end_i2c() {

  }

  void AudioAmpModule::write_i2c_byte(uint8_t number) {

  }

  uint8_t AudioAmpModule::read_i2c_byte() {
    return 0; // TODO
  }

  void AudioAmpModule::write_i2c_reg(uint8_t reg, uint8_t data) {

  }

  uint8_t AudioAmpModule::read_i2c_reg(uint8_t reg) {
    return 0; // TODO
  }

  uint8_t AudioAmpModule::detect_i2c() {
    return 0; // TODO
  }

}
