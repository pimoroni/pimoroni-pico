#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class BMP280 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x76;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x77;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;

  private:
    static const uint8_t CHIP_ID  = 0x58;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;

    /***** More variables here *****/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BMP280() {}

    BMP280(uint8_t address) :
      address(address) {}

    BMP280(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void setup(uint8_t mode/*='normal'*/, uint8_t temperature_oversampling/*=16*/,
      uint8_t pressure_oversampling/*=16*/, uint8_t temperature_standby/*=500*/);

    void update_sensor();
    uint8_t get_temperature();
    uint8_t get_pressure();
    uint8_t get_altitude(float qnh/* = 1013.25f*/, uint8_t manual_temperature/*=None*/);

  private:
    /***** Private methods here *****/
  };

}
