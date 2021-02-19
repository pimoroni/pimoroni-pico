#include "vl53l1x.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool VL53L1X::init() {
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

  uint8_t VL53L1X::set_address(uint8_t address) {
    return 0;
  }

  uint8_t VL53L1X::set_distance_mode(uint8_t mode) {
    return 0;
  }

  uint8_t VL53L1X::set_user_roi(uint8_t topLeftX, uint8_t topLeftY, uint8_t botRightX, uint8_t botRightY) {
    return 0;
  }

  uint8_t VL53L1X::start_ranging(uint8_t mode) {
    return 0;
  }

  uint8_t VL53L1X::set_measurement_timing_budget_microseconds(uint8_t timing_budget) {
    return 0;
  }

  uint8_t VL53L1X::set_inter_measurement_period_milliseconds(uint8_t period) {
    return 0;
  }

  int32_t VL53L1X::get_distance() {
    return 0;  
  }

  void VL53L1X::stop_ranging() {

  }
}