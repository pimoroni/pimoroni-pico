#pragma once

#include "../../drivers/is31fl3731/is31fl3731.hpp"

namespace pimoroni {

  class BreakoutMatrix11x7 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const int WIDTH    = 11;
    static const int HEIGHT   = 7;

    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x75;
    static const uint8_t I2C_ADDRESS_ALTERNATE    = 0x77;
    static const uint8_t DEFAULT_SDA_PIN          = 20;
    static const uint8_t DEFAULT_SCL_PIN          = 21;

  private:
    static const uint8_t LOOKUP_TABLE[WIDTH * HEIGHT];


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IS31FL3731 matrix;

    
    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutMatrix11x7() :
      matrix(DEFAULT_I2C_ADDRESS) {}

    BreakoutMatrix11x7(uint8_t address) :
      matrix(address) {}

    BreakoutMatrix11x7(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      matrix(i2c, address, sda, scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    void set_pixel(uint8_t x, uint8_t y, uint8_t brightness);
    void update();
    void clear();

  private:
    uint8_t lookup_pixel(uint8_t index);
  };

}
