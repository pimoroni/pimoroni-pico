#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <initializer_list>

namespace pimoroni {

  class IS31FL3731 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x74;
    static const uint8_t I2C_ADDRESS_ALTERNATE1   = 0x75;
    static const uint8_t I2C_ADDRESS_ALTERNATE2   = 0x76;
    static const uint8_t I2C_ADDRESS_ALTERNATE3   = 0x77;
    static const uint8_t DEFAULT_SDA_PIN          = 20;
    static const uint8_t DEFAULT_SCL_PIN          = 21;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;

    uint8_t buf[145];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    IS31FL3731() {}

    IS31FL3731(uint8_t address) :
      address(address) {}

    IS31FL3731(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    void enable(std::initializer_list<uint8_t> pattern, uint8_t frame = 0);
    void set(uint8_t index, uint8_t brightness);
    void update(uint8_t frame = 0);
    void clear();

  private:
    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    int16_t i2c_reg_read_int16(uint8_t reg);
  };

}
