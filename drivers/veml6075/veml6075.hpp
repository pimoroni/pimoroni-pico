#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class VEML6075 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x10;
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
    VEML6075() {}

    VEML6075(i2c_inst_t *i2c, uint8_t sda, uint8_t scl) :
      i2c(i2c), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    uint8_t get_integration_time();
    void set_integration_time(uint8_t value);
    uint8_t get_id();
    uint8_t get_shutdown();
    void set_shutdown(uint8_t value);
    void set_high_dynamic_range(uint8_t value);
    uint8_t get_measurements();
    uint8_t get_comparitor_readings();
    uint8_t convert_to_index(uint8_t uva, uint8_t uvb, uint8_t uv_comp1, uint8_t uv_comp2);

  private:
    /***** Private methods here *****/
  };

}
