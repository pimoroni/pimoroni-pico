#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"
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


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;

    uint8_t buf[145];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    IS31FL3731() {}

    IS31FL3731(uint8_t address) : IS31FL3731(new I2C(I2C_DEFAULT_SDA, I2C_DEFAULT_SCL), address) {}
  
    IS31FL3731(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS) : i2c(i2c), address(address) {}

    IS31FL3731(i2c_inst_t *i2c_inst, uint8_t address, uint8_t sda, uint8_t scl) :
      address(address) {
        i2c = new I2C(sda, scl);
      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;

    void enable(std::initializer_list<uint8_t> pattern, uint8_t frame = 0);
    void set(uint8_t index, uint8_t brightness);
    void update(uint8_t frame = 0);
    void clear();

  private:
    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    int16_t i2c_reg_read_int16(uint8_t reg);
  };

}
