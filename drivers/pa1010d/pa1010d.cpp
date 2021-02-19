#include "pa1010d.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool PA1010D::init() {
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

  uint8_t PA1010D::data() {
    return 0;
  }

  void PA1010D::send_command(uint8_t command, bool add_checksum) {
    
  }

  uint8_t PA1010D::read_sentence(uint8_t timeout) {
    return 0;
  }

  bool PA1010D::update(uint8_t wait_for/*="GGA"*/, uint8_t timeout) {
    return false;
  }

  void PA1010D::write_sentence(uint8_t bytestring) {

  }
}