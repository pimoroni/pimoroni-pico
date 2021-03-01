#pragma once

#include "../../drivers/is31fl3731/is31fl3731.hpp"

namespace pimoroni {

  struct RGBLookup {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  class BreakoutRGBMatrix5x5 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const int WIDTH    = 5;
    static const int HEIGHT   = 5;

    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x74;
    static const uint8_t I2C_ADDRESS_ALTERNATE    = 0x77;
    static const uint8_t DEFAULT_SDA_PIN          = 20;
    static const uint8_t DEFAULT_SCL_PIN          = 21;

  private:
    static const RGBLookup LOOKUP_TABLE[WIDTH * HEIGHT];


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IS31FL3731 matrix;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutRGBMatrix5x5() :
      matrix(DEFAULT_I2C_ADDRESS) {}

    BreakoutRGBMatrix5x5(uint8_t address) :
      matrix(address) {}

    BreakoutRGBMatrix5x5(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      matrix(i2c, address, sda, scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    i2c_inst_t* get_i2c() const;
    int get_sda() const;
    int get_scl() const;

    void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void update();
    void clear();

  private:
    RGBLookup lookup_pixel(uint8_t index);
  };

}
