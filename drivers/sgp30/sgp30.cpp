#include "sgp30.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool SGP30::init() {
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

  uint8_t SGP30::command(uint8_t command_name, uint8_t parameters/*=None*/) {
    return 0;
  }

  uint8_t SGP30::calculate_crc(uint8_t data) {
    return 0;
  }

  uint8_t SGP30::get_unique_id() {
    return 0;
  }

  void SGP30::start_measurement(uint8_t run_while_waiting/*=None*/) {

  }

  uint8_t SGP30::get_air_quality() {
    return 0;
  }

  void SGP30::set_baseline(uint8_t eco2, uint8_t tvoc) {

  }
}