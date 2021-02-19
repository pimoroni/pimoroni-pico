#include "veml6075.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool VEML6075::init() {
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

  uint8_t VEML6075::get_integration_time() {
    return 0;
  }

  void VEML6075::set_integration_time(uint8_t value) {

  }

  uint8_t VEML6075::get_id() {
    return 0;
  }

  uint8_t VEML6075::get_shutdown() {
    return 0;
  }

  void VEML6075::set_shutdown(uint8_t value) {

  }

  void VEML6075::set_high_dynamic_range(uint8_t value) {

  }

  uint8_t VEML6075::get_measurements() {
    return 0;
  }

  uint8_t VEML6075::get_comparitor_readings() {
    return 0;
  }

  uint8_t VEML6075::convert_to_index(uint8_t uva, uint8_t uvb, uint8_t uv_comp1, uint8_t uv_comp2) {
    return 0;
  }
}