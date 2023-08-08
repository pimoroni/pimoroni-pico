#include "audio_amp.hpp"
#include "../../logging.hpp"
#include "../../errors.hpp"
#include "tas_regs.hpp"

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
    YukonModule(),
    slow_sda(nullptr),
    slow_scl(nullptr),
    amp_en(nullptr) {
  }

  AudioAmpModule::~AudioAmpModule() {
    logging.debug("[AudioAmpModule] Destructor\n");
  }

  std::string AudioAmpModule::name() {
    return AudioAmpModule::NAME;
  }

  void AudioAmpModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create the "I2C" pin objects
    slow_sda = new TCA_IO(slot.SLOW1, accessor);
    slow_scl = new TCA_IO(slot.SLOW2, accessor);

    // Create the enable pin object
    amp_en = new TCA_IO(slot.SLOW3, accessor);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void AudioAmpModule::reset() {
    logging.debug("[AudioAmpModule] Resetting\n");
    CHECK_INITIALISED
    slow_sda->to_output(true);
    slow_scl->to_output(true);
    amp_en->to_output(false);
  }

  void AudioAmpModule::enable() {
    CHECK_INITIALISED
    amp_en->value(true);

    // Pre-Reset Configuration
    write_i2c_reg(PAGE, 0x01);  // Page 1
    write_i2c_reg(0x37, 0x3a);  // Bypass

    write_i2c_reg(PAGE, 0xFD);  // Page FD
    write_i2c_reg(0x0D, 0x0D);  // Access page
    write_i2c_reg(0x06, 0xC1);  // Set Dmin

    write_i2c_reg(PAGE, 0x01);  // Page 1
    write_i2c_reg(0x19, 0xC0);  // Force modulation
    write_i2c_reg(PAGE, 0xFD);  // Page FD
    write_i2c_reg(0x0D, 0x0D);  // Access page
    write_i2c_reg(0x06, 0xD5);  // Set Dmin

    // Software Reset
    write_i2c_reg(PAGE, 0x00);  // Page 0
    write_i2c_reg(0x7F, 0x00);  // Book 0
    write_i2c_reg(0x01, 0x01);  // Software Reset

    // Post-Reset Configuration
    write_i2c_reg(PAGE, 0x01);  // Page 1
    write_i2c_reg(0x37, 0x3a);  // Bypass

    write_i2c_reg(PAGE, 0xFD);  // Page FD
    write_i2c_reg(0x0D, 0x0D);  // Access page
    write_i2c_reg(0x06, 0xC1);  // Set Dmin
    write_i2c_reg(0x06, 0xD5);  // Set Dmin

    // Initial Device Configuration - PWR_MODE0
    write_i2c_reg(PAGE, 0x00);  // Page 0
    write_i2c_reg(0x0E, 0x44);  // TDM tx vsns transmit enable with slot 4
    write_i2c_reg(0x0F, 0x40);  // TDM tx isns transmit enable with slot 0

    write_i2c_reg(PAGE, 0x01);  // Page 1
    write_i2c_reg(0x21, 0x00);  // Disable Comparator Hysterisis
    write_i2c_reg(0x17, 0xC8);  // SARBurstMask=0
    write_i2c_reg(0x19, 0x00);  // LSR Mode
    write_i2c_reg(0x35, 0x74);  // Noise minimized

    write_i2c_reg(PAGE, 0xFD);  // Page FD
    write_i2c_reg(0x0D, 0x0D);  // Access page
    write_i2c_reg(0x3E, 0x4A);  // Optimal Dmin
    write_i2c_reg(0x0D, 0x00);  // Remove access

    write_i2c_reg(PAGE, 0x00);    // Page 0
    write_i2c_reg(CHNL_0, 0xA8);  // PWR_MODE0 selected
    write_i2c_reg(PVDD_UVLO, 0x00);   // PVDD UVLO set to 2.76V
    // My addition
    write_i2c_reg(DC_BLK0, 0xA1);   // VBAT1S_MODE set to internally generated
    write_i2c_reg(DVC, 0x68);   // Go to a low default
    write_i2c_reg(INT_CLK_CFG, 0x99 + 0b0100000);   // CLK_PWR_UD_EN abled, with long time. This causes output to stay active without mute.

    write_i2c_reg(INT_MASK0, 0xFF);
    write_i2c_reg(INT_MASK1, 0xFF);
    write_i2c_reg(INT_MASK2, 0xFF);
    write_i2c_reg(INT_MASK3, 0xFF);
    write_i2c_reg(INT_MASK4, 0xFF);

    write_i2c_reg(MODE_CTRL, 0x80);   // Play audio, power up with playback, IV enabled
    // A second play command is required for some reason, to take it out of software shutdown
    // Temp commented out self.write_i2c_reg(MODE_CTRL, 0x80) # Play audio, power up with playback, IV enabled
  }

  void AudioAmpModule::disable() {
    CHECK_INITIALISED
    amp_en->value(false);
  }

  bool AudioAmpModule::is_enabled() {
    CHECK_INITIALISED
    return amp_en->value();
  }

  void AudioAmpModule::exit_soft_shutdown() {
    CHECK_INITIALISED
    write_i2c_reg(MODE_CTRL, 0x80);   // Calling this after a play seems to wake the amp up, but adds around 16ms
  }

  void AudioAmpModule::set_volume(float volume) {
    if(volume < 0.0f || volume > 1.0f) {
      throw std::invalid_argument("Volume out of range. Expected 0.0 to 1.0\n");
    }

    write_i2c_reg(DVC, (uint8_t)((1.0f - volume) * 0xC8));
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
