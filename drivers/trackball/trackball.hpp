#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class Trackball {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS        = 0x0A;
    static const uint8_t I2C_ADDRESS_ALTERNATIVE    = 0x0B;
    static const uint32_t DEFAULT_TIMEOUT           = 5;

  private:
    static const uint16_t CHIP_ID = 0xBA11;
    static const uint8_t VERSION = 1;


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct State {
      uint8_t left;
      uint8_t right;
      uint8_t up;
      uint8_t down;
      bool sw_changed;
      bool sw_pressed;
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;
    int8_t address      = DEFAULT_I2C_ADDRESS;
    uint interrupt      = PIN_UNUSED;
    uint32_t timeout    = DEFAULT_TIMEOUT;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Trackball(uint8_t address = DEFAULT_I2C_ADDRESS) : i2c(new I2C()), address(address) {};

    Trackball(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT) : i2c(i2c), address(address), interrupt(interrupt) {}

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public: 
    bool init();

    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    void change_address(uint8_t new_address);
        
    void enable_interrupt(bool use_interrupt = true);
    bool get_interrupt();
    void set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void set_red(uint8_t value);
    void set_green(uint8_t value);
    void set_blue(uint8_t value);
    void set_white(uint8_t value);
    State read();

  private:
    void wait_for_flash();

    uint32_t millis();
  };

}
