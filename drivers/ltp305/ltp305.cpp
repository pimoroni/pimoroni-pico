#include "ltp305.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool LTP305::init() {
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

  void LTP305::clear() {

  }

  void LTP305::set_brightness(uint8_t brightness, bool update) {

  }

  void LTP305::set_decimal(uint8_t left, uint8_t right) {

  }

  void LTP305::set_pixel(uint8_t x, uint8_t y, uint8_t c) {

  }

  void LTP305::set_character(uint8_t x, uint8_t ch) {

  }

  uint8_t LTP305::get_shape() {
    return 0;
  }

  void LTP305::set_image(void *image, uint8_t offset_x, uint8_t offset_y, bool wrap, uint8_t bg) {

  }

  void LTP305::show() {

  }
}