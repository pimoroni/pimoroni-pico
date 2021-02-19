#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class SGP30 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x58;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;

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

    /***** More variables here *****/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    SGP30() {}

    SGP30(i2c_inst_t *i2c, uint8_t sda, uint8_t scl) :
      i2c(i2c), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    uint8_t command(uint8_t command_name, uint8_t parameters/*=None*/);
    uint8_t calculate_crc(uint8_t data);
    uint8_t get_unique_id();
    void start_measurement(uint8_t run_while_waiting/*=None*/);
    uint8_t get_air_quality();
    void set_baseline(uint8_t eco2, uint8_t tvoc);

  private:
    /***** Private methods here *****/
  };

}
