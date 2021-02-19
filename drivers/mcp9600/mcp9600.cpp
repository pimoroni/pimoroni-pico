#include "mcp9600.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool MCP9600::init() {
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

  void set_thermocouple_type(uint8_t thermocouple_type) {

  }

  uint8_t get_thermocouple_type() {
    return 0;
  }

  uint8_t get_hot_junction_temperature() {
    return 0;
  }

  uint8_t get_cold_junction_temperature() {
    return 0;
  }

  uint8_t get_temperature_delta() {
    return 0;
  }

  uint8_t check_alerts() {
    return 0;
  }

  void clear_alert(uint8_t index) {

  }

  uint8_t get_alert_hysteresis(uint8_t index) {
    return 0;
  }

  uint8_t get_alert_limit(uint8_t index) {
    return 0;
  }

  void configure_alert(uint8_t index, uint8_t limit, uint8_t hysteresis, bool clear_interrupt,
    uint8_t monitor_junction, uint8_t rise_fall, uint8_t state, uint8_t mode, bool enable) {

  }
}