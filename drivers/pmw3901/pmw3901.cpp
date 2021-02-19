#include "pmw3901.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool PMW3901::init() {
    bool succeeded = false;

    spi_init(spi, 400000);

    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_pull_up(sck);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_pull_up(mosi);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_pull_up(miso);
    gpio_set_function(cs, GPIO_FUNC_SPI);
    gpio_pull_up(cs);

    gpio_set_function(interrupt, GPIO_FUNC_SIO);
    gpio_set_dir(interrupt, GPIO_IN);
    gpio_pull_up(interrupt);

    /***** Replace if(true) with any operations needed to initialise the device *****/
    if(true) {
      succeeded = true;
    }

    return succeeded;
  }

  uint8_t PMW3901::get_id(void) {
    return 0;
  }

  void PMW3901::set_rotation(float degrees) {

  }

  void PMW3901::set_orientation(bool invert_x, bool invert_y, bool swap_xy) {

  }

  uint8_t PMW3901::get_motion(uint8_t timeout) {
    return 0;
  }

  uint8_t PMW3901::get_motion_slow(uint8_t timeout) {
    return 0;
  }

  void PMW3901::write(uint8_t reg, uint8_t value) {

  }

  uint8_t PMW3901::read(uint8_t reg, uint8_t length) {
    return 0;
  }

  void PMW3901::bulk_write(uint8_t data) {

  }

  void PMW3901::secret_sauce() {

  }
}