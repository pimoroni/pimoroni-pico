#include "max30105.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool MAX30105::init() {
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

  void setup(float led_power, uint8_t sample_average, uint8_t leds_enable,
    uint16_t sample_rate, uint16_t pulse_width, uint16_t adc_range, float timeout) {

  }

  void soft_reset(float timeout) {

  }

  void clear_fifo() {

  }

  uint8_t get_samples() {
    return 0;
  }

  uint8_t get_chip_id() {
    return 0;
  }

  uint8_t get_temperature(float timeout) {
    return 0;
  }

  void set_mode(uint8_t mode) {

  }

  void set_slot_mode(uint8_t slot, uint8_t mode) {

  }

  void set_led_pulse_amplitude(uint8_t led, uint8_t amplitude) {

  }

  void set_fifo_almost_full_count(uint8_t count) {

  }

  void set_fifo_almost_full_enable(uint8_t value) {

  }

  void set_data_ready_enable(uint8_t value) {

  }

  void set_ambient_light_compensation_overflow_enable(uint8_t value) {

  }

  void set_proximity_enable(uint8_t value) {

  }

  void set_proximity_threshold(uint8_t value) {
  
  }

  uint8_t get_fifo_almost_full_status() {
    return 0;
  }

  uint8_t get_data_ready_status() {
    return 0;
  }

  uint8_t get_ambient_light_compensation_overflow_status() {
    return 0;
  }

  uint8_t get_proximity_triggered_threshold_status() {
    return 0;
  }

  uint8_t get_power_ready_status() {
    return 0;
  }

  uint8_t get_die_temp_ready_status() {
    return 0;
  }
}