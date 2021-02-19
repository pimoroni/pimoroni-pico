#include "bme280.hpp"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool BME280::init() {
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


  void BME280::setup(uint8_t mode/*='normal'*/, uint16_t temperature_oversampling/*=16*/, uint16_t pressure_oversampling/*=16*/,
    uint16_t humidity_oversampling/*=16*/, uint16_t temperature_standby/*=500*/) {
    
  }

  void BME280::update_sensor() {

  }

  float BME280::get_temperature() {
    return 0;
  }

  float BME280::get_pressure() {
    return 0;
  }

  float BME280::get_humidity() {
    return 0;
  }

  float BME280::get_altitude(float qnh) {
    return 0;
  }
}