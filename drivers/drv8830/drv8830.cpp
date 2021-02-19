#include "drv8830.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool DRV8830::init() {
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

  void DRV8830::select_i2c_address(uint8_t i2c_addr) {

  }
  
  void DRV8830::set_outputs(bool out1, bool out2) {

  }

  void DRV8830::brake() {

  }

  void DRV8830::coast() {

  }

  void DRV8830::forward() {

  }

  void DRV8830::reverse() {

  }

  void DRV8830::set_direction(uint8_t direction) {

  }

  void DRV8830::set_voltage(uint8_t voltage) {
    
  }

  uint8_t DRV8830::get_voltage() {
    return 0;  
  }

  uint8_t DRV8830::get_fault() {
    return 0;
  }

  void DRV8830::clear_fault() {

  }
}