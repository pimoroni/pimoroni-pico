#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"

#include "common/pimoroni_i2c.hpp"

#include "libraries/pico_graphics/pico_graphics.hpp"

#include <algorithm>


namespace pimoroni {

  class SH1107 : public DisplayDriver {
    I2C &i2c;
  
  public:
    bool round;
    static const uint8_t DEFAULT_I2C_ADDRESS = 0x3c;
    static const uint8_t ALTERNATE_I2C_ADDRESS = 0x3d;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint8_t i2c_address = DEFAULT_I2C_ADDRESS;

  public:
    SH1107(uint16_t width, uint16_t height, I2C &i2c) : SH1107(width, height, i2c, DEFAULT_I2C_ADDRESS) {};
      
    SH1107(uint16_t width, uint16_t height, I2C &i2c, uint8_t i2c_address) :
      DisplayDriver(width, height, ROTATE_0),
      i2c(i2c), i2c_address(i2c_address) {
      
      i2c.reg_write_uint8(i2c_address, 0, 0xae);

      i2c.reg_write_uint8(i2c_address, 0, 0x20); // set memory addressing mode
      i2c.reg_write_uint8(i2c_address, 0, 0x00);

      i2c.reg_write_uint8(i2c_address, 0, 0xb0); // set page start address

      i2c.reg_write_uint8(i2c_address, 0, 0xc0); // mirror vertically (for australian market)

      i2c.reg_write_uint8(i2c_address, 0, 0x00);
      i2c.reg_write_uint8(i2c_address, 0, 0x10);

      i2c.reg_write_uint8(i2c_address, 0, 0x40);

      i2c.reg_write_uint8(i2c_address, 0, 0xa0); // mirror horizontally

      i2c.reg_write_uint8(i2c_address, 0, 0xa6); // no inversion

      i2c.reg_write_uint8(i2c_address, 0, 0xff); // ??????!
      i2c.reg_write_uint8(i2c_address, 0, 0x3f); // confusion intensifies..

      i2c.reg_write_uint8(i2c_address, 0, 0xa4);

      i2c.reg_write_uint8(i2c_address, 0, 0xd3); // set display offset
      i2c.reg_write_uint8(i2c_address, 0, 0x00);

      i2c.reg_write_uint8(i2c_address, 0, 0xd5); // set display clock divide
      i2c.reg_write_uint8(i2c_address, 0, 0xf0);

      i2c.reg_write_uint8(i2c_address, 0, 0xd9); // set precharge period
      i2c.reg_write_uint8(i2c_address, 0, 0x22);

      i2c.reg_write_uint8(i2c_address, 0, 0xda); // set com pins hardware configuration
      i2c.reg_write_uint8(i2c_address, 0, 0x12);

      i2c.reg_write_uint8(i2c_address, 0, 0xdb); // set vcomh
      i2c.reg_write_uint8(i2c_address, 0, 0x20);

      i2c.reg_write_uint8(i2c_address, 0, 0x8d); // set dc-dc enable
      i2c.reg_write_uint8(i2c_address, 0, 0x14);

      i2c.reg_write_uint8(i2c_address, 0, 0xaf); // turn display on
    }

    void update(PicoGraphics *graphics) override;

  private:
    void common_init();
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
