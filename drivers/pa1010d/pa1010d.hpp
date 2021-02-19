#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class PA1010D {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x10;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;

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
    PA1010D() {}

    PA1010D(uint8_t address) :
      address(address) {}

    PA1010D(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}



    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    uint8_t data();    
    void send_command(uint8_t command, bool add_checksum = true);
    uint8_t read_sentence(uint8_t timeout = 5);
    bool update(uint8_t wait_for/*="GGA"*/, uint8_t timeout = 5);

  private:
    void write_sentence(uint8_t bytestring);
  };

}
