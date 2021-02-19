#include "bme680.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool BME680::init() {
    bool succeeded = false;

    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    /***** Replace if(true) with any operations needed to initialise the device *****/
    if(true) {
      succeeded = true;
    }

    return succeeded;
  }

  void BME680::soft_reset() {

  }

  void BME680::set_temp_offset(uint8_t value) {

  }

  void BME680::set_humidity_oversample(uint8_t value) {

  }

  uint8_t BME680::get_humidity_oversample() {

  }

  void BME680::set_pressure_oversample(uint8_t value) {

  }

  uint8_t BME680::get_pressure_oversample() {

  }

  void BME680::set_temperature_oversample(uint8_t value) {

  }

  uint8_t BME680::get_temperature_oversample() {

  }

  void BME680::set_filter(uint8_t value) {

  }

  uint8_t BME680::get_filter() {

  }

  void BME680::select_gas_heater_profile(uint8_t value) {

  }

  uint8_t BME680::get_gas_heater_profile() {

  }

  void BME680::set_gas_status(uint8_t value) {

  }

  uint8_t BME680::get_gas_status() {

  }

  void BME680::set_gas_heater_profile(uint8_t temperature, uint8_t duration, uint8_t nb_profile) {

  }

  void BME680::set_gas_heater_temperature(uint8_t value, uint8_t nb_profile) {

  }

  void BME680::set_gas_heater_duration(uint8_t value, uint8_t nb_profile) {

  }

  void BME680::set_power_mode(uint8_t value, bool blocking) {

  }

  uint8_t BME680::get_power_mode() {
    return 0;
  }

  bool BME680::get_sensor_data() {
    return false;
  }
  
  void BME680::get_calibration_data() {

  }

  void BME680::set_bits(uint8_t reg, uint8_t mask, uint8_t position, uint8_t value) {

  }

  void BME680::set_regs(uint8_t reg, uint8_t value) {

  }

  uint32_t BME680::get_regs(uint8_t reg, uint8_t length) {
    return 0;
  }

  uint32_t BME680::calc_temperature(uint32_t temperature_adc) {
    return 0;
  }

  uint32_t BME680::calc_pressure(uint32_t pressure_adc) {
    return 0;
  }

  uint32_t BME680::calc_humidity(uint32_t humidity_adc) {
    return 0;
  }

  uint32_t BME680::calc_gas_resistance(uint32_t gas_res_adc, uint32_t gas_range) {
    return 0;
  }

  uint16_t BME680::calc_heater_resistance(uint16_t temperature) {
    return 0;
  }

  uint8_t BME680::calc_heater_duration(uint16_t duration) {
    return 0;
  }
}