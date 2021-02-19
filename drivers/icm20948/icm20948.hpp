#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class ICM20948 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x68;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x69;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;
    static const uint8_t DEFAULT_INT_PIN        = 22;
    static const uint8_t PIN_UNUSED             = UINT8_MAX;

    /***** More public constants here *****/

  private:
    /***** Private constants here *****/


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;
    int8_t interrupt  = DEFAULT_INT_PIN;

    /***** More variables here *****/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ICM20948() {}

    ICM20948(uint8_t address) :
      address(address) {}

    ICM20948(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void write(uint8_t reg, uint8_t value);
    uint8_t read(uint8_t reg);
    void trigger_mag_io();
    uint8_t read_bytes(uint8_t reg, uint8_t length = 1);
    void bank(uint8_t value);
    uint8_t mag_write(uint8_t reg, uint8_t value);
    uint8_t mag_read(uint8_t reg);
    uint8_t mag_read_bytes(uint8_t reg, uint8_t length = 1);
    uint8_t magnetometer_ready();
    uint8_t read_magnetometer_data(float timeout = 1.0f);
    uint8_t read_accelerometer_gyro_data();
    void set_accelerometer_sample_rate(uint8_t rate = 125);
    void set_accelerometer_full_scale(uint8_t scale = 16);
    void set_accelerometer_low_pass(bool enabled = true, uint8_t mode = 5);
    void set_gyro_sample_rate(uint8_t rate = 100);
    void set_gyro_full_scale(uint8_t scale = 250);
    void set_gyro_low_pass(bool enabled = true, uint8_t mode = 5);
    uint8_t read_temperature();

  private:
    /***** Private methods here *****/
  };

}
