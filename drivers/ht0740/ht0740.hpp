#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class HT0740 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x38;
    static const uint8_t I2C_ADDRESS_ALTERNATE1   = 0x39;
    static const uint8_t I2C_ADDRESS_ALTERNATE2   = 0x40;
    static const uint8_t I2C_ADDRESS_ALTERNATE3   = 0x41;
    static const uint8_t DEFAULT_SDA_PIN          = 20;
    static const uint8_t DEFAULT_SCL_PIN          = 21;
    static const uint8_t DEFAULT_INT_PIN          = 22;
    static const uint8_t PIN_UNUSED               = UINT8_MAX;

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
    HT0740() {}

    HT0740(uint8_t address) :
      address(address) {}

    HT0740(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void on();
    void off();
    void disable();
    void enable();
    void toggle();

  private:
    /***** Private methods here *****/
  };

}
