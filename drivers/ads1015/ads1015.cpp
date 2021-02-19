#include "ads1015.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool ADS1015::init() {
    bool succeeded = false;

    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    /***** Replace if(true) with any operations needed to initialise the device *****/
    if(true) {
      succeeded = true;
    }

    return succeeded;
  }

  void ADS1015::start_conversion() {

  }

  bool ADS1015::conversion_ready() {
    return false;
  }

  void ADS1015::set_status(uint8_t value) {

  }

  uint8_t ADS1015::get_status() {
    return 0;
  }

  void ADS1015::set_multiplexer(uint8_t value) {

  }

  uint8_t ADS1015::get_multiplexer() {
    return 0;
  }

  void ADS1015::set_mode(uint8_t value) {

  }

  uint8_t ADS1015::get_mode() {
    return 0;
  }

  void ADS1015::set_programmable_gain(float value) {

  }

  float ADS1015::get_programmable_gain() {
    return 0;
  }

  void ADS1015::set_sample_rate(uint16_t value) {

  }

  uint16_t ADS1015::get_sample_rate() {
    return 0;
  }

  void ADS1015::set_comparator_mode(uint8_t value) {

  }

  uint8_t ADS1015::get_comparator_mode() {
    return 0;
  }

  void ADS1015::set_comparator_latching(uint8_t value) {

  }

  uint8_t ADS1015::get_comparator_latching() {
    return 0;
  }

  void ADS1015::set_comparator_queue(uint8_t value) {

  }

  uint8_t ADS1015::get_comparator_queue() {
    return 0;
  }

  void ADS1015::wait_for_conversion(uint16_t timeout) {

  }
  
  uint8_t ADS1015::get_reference_voltage() {
    return 0;
  }

  uint8_t ADS1015::get_voltage(uint8_t channel) {
    return 0;
  }

  uint8_t ADS1015::get_compensated_voltage(uint8_t channel, uint32_t vdiv_a, uint32_t vdiv_b, float reference_voltage) {
    return 0;
  }

  void ADS1015::get_conversion_value() {

  }

  void ADS1015::set_low_threshold(uint8_t value) {

  }

  uint8_t ADS1015::get_low_threshold() {
    return 0;
  }

  void ADS1015::set_high_threshold(uint8_t value) {

  }

  uint8_t ADS1015::get_high_threshold() {
    return 0;
  }
}