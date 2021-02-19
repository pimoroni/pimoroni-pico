#include "bmp280.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool BMP280::init() {
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


  void BMP280::setup(uint8_t mode/*='normal'*/, uint8_t temperature_oversampling/*=16*/,
    uint8_t pressure_oversampling/*=16*/, uint8_t temperature_standby/*=500*/) {
    
  }

  void BMP280::update_sensor() {

  }

  uint8_t BMP280::get_temperature() {
    return 0;
  }

  uint8_t BMP280::get_pressure() {
    return 0;
  }

  uint8_t BMP280::get_altitude(float qnh/* = 1013.25f*/, uint8_t manual_temperature/*=None*/) {
    return 0;
  }
}