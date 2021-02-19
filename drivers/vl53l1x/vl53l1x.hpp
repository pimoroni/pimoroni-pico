#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class VL53L1X {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x29;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;
    static const uint8_t DEFAULT_INT_PIN      = 22;
    static const uint8_t PIN_UNUSED           = UINT8_MAX;

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
    VL53L1X() {}

    VL53L1X(uint8_t address) :
      address(address) {}

    VL53L1X(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    uint8_t set_address(uint8_t address);
    uint8_t set_distance_mode(uint8_t mode);
    uint8_t set_user_roi(uint8_t topLeftX, uint8_t topLeftY, uint8_t botRightX, uint8_t botRightY);
    uint8_t start_ranging(uint8_t mode);
    uint8_t set_measurement_timing_budget_microseconds(uint8_t timing_budget);
    uint8_t set_inter_measurement_period_milliseconds(uint8_t period);
    int32_t get_distance();
    void stop_ranging();

    /***** More public methods here *****/

  private:
    /***** Private methods here *****/
  };

}
