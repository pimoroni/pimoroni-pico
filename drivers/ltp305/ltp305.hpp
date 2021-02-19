#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class LTP305 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x61;
    static const uint8_t I2C_ADDRESS_ALTERNATE1   = 0x62;
    static const uint8_t I2C_ADDRESS_ALTERNATE2   = 0x63;
    static const uint8_t DEFAULT_SDA_PIN          = 20;
    static const uint8_t DEFAULT_SCL_PIN          = 21;

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
    LTP305() {}

    LTP305(uint8_t address) :
      address(address) {}

    LTP305(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void clear();
    void set_brightness(uint8_t brightness, bool update = false);
    void set_decimal(uint8_t left/*=None*/, uint8_t right/*=None*/);
    void set_pixel(uint8_t x, uint8_t y, uint8_t c);
    void set_character(uint8_t x, uint8_t ch);
    uint8_t get_shape();
    void set_image(void *image, uint8_t offset_x = 0, uint8_t offset_y = 0, bool wrap = false, uint8_t bg = 0);
    void show();

  private:
    /***** Private methods here *****/
  };

}
