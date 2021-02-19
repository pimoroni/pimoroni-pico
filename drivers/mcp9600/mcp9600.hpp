#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class MCP9600 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x66;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x67;
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
    MCP9600() {}

    MCP9600(uint8_t address) :
      address(address) {}

    MCP9600(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void set_thermocouple_type(uint8_t thermocouple_type);
    uint8_t get_thermocouple_type();
    uint8_t get_hot_junction_temperature();
    uint8_t get_cold_junction_temperature();
    uint8_t get_temperature_delta();
    uint8_t check_alerts();
    void clear_alert(uint8_t index);
    uint8_t get_alert_hysteresis(uint8_t index);
    uint8_t get_alert_limit(uint8_t index);
    void configure_alert(uint8_t index, uint8_t limit = 0/*=None*/, uint8_t hysteresis = 0/*=None*/, bool clear_interrupt = true,
      uint8_t monitor_junction = 0, uint8_t rise_fall = 1, uint8_t state = 1, uint8_t mode = 1, bool enable = false);

  private:
    /***** Private methods here *****/
  };

}
