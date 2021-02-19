#include "lsm303d.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool LSM303D::init() {
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

  void LSM303D::setup() {

  }

  void LSM303D::set_accel_full_scale_g(uint8_t scale) {

  }

  void LSM303D::set_mag_full_scale_guass(uint8_t scale) {

  }

  uint8_t LSM303D::get_magnetometer() {
    return 0;
  }

  uint8_t LSM303D::get_accelerometer() {
    return 0;
  }

  uint8_t LSM303D::get_temperature() {
    return 0;
  }
}