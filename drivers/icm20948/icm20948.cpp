#include "icm20948.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool ICM20948::init() {
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

  void ICM20948::write(uint8_t reg, uint8_t value) {

  }

  uint8_t ICM20948::read(uint8_t reg) {
    return 0;
  }

  void ICM20948::trigger_mag_io() {

  }

  uint8_t ICM20948::read_bytes(uint8_t reg, uint8_t length) {
    return 0;
  }

  void ICM20948::bank(uint8_t value) {

  }

  uint8_t ICM20948::mag_write(uint8_t reg, uint8_t value) {
    return 0;
  }

  uint8_t ICM20948::mag_read(uint8_t reg) {
    return 0;
  }

  uint8_t ICM20948::mag_read_bytes(uint8_t reg, uint8_t length) {
    return 0;
  }

  uint8_t ICM20948::magnetometer_ready() {
    return 0;
  }

  uint8_t ICM20948::read_magnetometer_data(float timeout) {
    return 0;
  }

  uint8_t ICM20948::read_accelerometer_gyro_data() {
    return 0;
  }

  void ICM20948::set_accelerometer_sample_rate(uint8_t rate) {

  }

  void ICM20948::set_accelerometer_full_scale(uint8_t scale) {

  }

  void ICM20948::set_accelerometer_low_pass(bool enabled, uint8_t mode) {

  }

  void ICM20948::set_gyro_sample_rate(uint8_t rate) {

  }

  void ICM20948::set_gyro_full_scale(uint8_t scale) {

  }

  void ICM20948::set_gyro_low_pass(bool enabled, uint8_t mode) {

  }

  uint8_t ICM20948::read_temperature() {
    return 0;
  }
}