#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class LTP305 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr uint8_t WIDTH = 10;
    static constexpr uint8_t HEIGHT = 7;

    static const uint8_t DEFAULT_I2C_ADDRESS      = 0x61;
    static const uint8_t I2C_ADDRESS_ALTERNATE1   = 0x62;
    static const uint8_t I2C_ADDRESS_ALTERNATE2   = 0x63;

    static const uint8_t DEFAULT_BRIGHTNESS       = 64;
    static const uint8_t MAX_BRIGHTNESS           = 127;
    static const uint8_t DEFAULT_ON_LEVEL         = 0x7f;

  private:
    static const uint8_t MODE = 0b00011000;
    static const uint8_t OPTS = 0b00001110;   // 1110 = 35mA, 0000 = 40mA

    static const uint8_t CMD_BRIGHTNESS = 0x19;
    static const uint8_t CMD_MODE = 0x00;
    static const uint8_t CMD_UPDATE = 0x0C;
    static const uint8_t CMD_OPTIONS = 0x0D;

    static const uint8_t CMD_MATRIX_L = 0x0E;
    static const uint8_t CMD_MATRIX_R = 0x01;

    static const uint8_t BUFFER_LENGTH = 8;
    static const uint8_t BUFFER_CMD = 1;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;

    uint8_t brightness = DEFAULT_BRIGHTNESS;

    //Including CMD in buffer to make transmission easier later on
    uint8_t buf_matrix_left[BUFFER_CMD + BUFFER_LENGTH] = { CMD_MATRIX_L, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t buf_matrix_right[BUFFER_CMD + BUFFER_LENGTH] = { CMD_MATRIX_R, 0, 0, 0, 0, 0, 0, 0, 0 };

    /* How matrix buffers look with all leds illuminated
    buf_matrix_left = {
    // Row 7654321
        0b01111111,  // col 1, bottom = msb
        0b01111111,  // col 2
        0b01111111,  // col 3
        0b01111111,  // col 4
        0b01111111,  // col 5
        0b00000000,
        0b00000000,
        0b01000000   # bit 7 = decimal dot
    };

    buf_matrix_right = {
    // Col   12345
        0b00011111,  // row 1
        0b00011111,  // row 2
        0b00011111,  // row 3
        0b00011111,  // row 4
        0b00011111,  // row 5
        0b00011111,  // row 6
        0b10011111,  // row 7 + bit 8 = decimal dot
        0b00000000
    };*/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    LTP305() {}

    LTP305(uint8_t address) :
      address(address) {}

    LTP305(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS) :
      i2c(i2c), address(address) {}

    LTP305(uint8_t address, uint sda, uint scl) : LTP305(new I2C(sda, scl), address) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;

    void set_brightness(uint8_t brightness, bool update = false);
    void set_decimal(bool left = false, bool right = false);
    void set_pixel(uint8_t x, uint8_t y, bool c);
    void set_character(uint8_t x, uint16_t ch);
    void set_image(const uint8_t *image, uint16_t image_width, uint16_t image_height,
                   uint16_t offset_x, uint16_t offset_y, bool wrap = false, bool bg = false, uint8_t on_level = DEFAULT_ON_LEVEL, uint8_t padding = 0);
    void clear();
    void show();
  };

}
