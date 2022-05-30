#pragma once

#include "drivers/ioexpander/ioexpander.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class BreakoutEncoder {
    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum Direction : bool {
      DIRECTION_CW = true,
      DIRECTION_CCW = false
    };


    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x0F;
    static constexpr float DEFAULT_BRIGHTNESS = 1.0f; //Effectively the maximum fraction of the period that the LED will be on
    static const Direction DEFAULT_DIRECTION  = DIRECTION_CW;
    static const uint32_t DEFAULT_TIMEOUT     = 1;

  private:
    static const uint8_t LED_R        = 1;
    static const uint8_t LED_G        = 7;
    static const uint8_t LED_B        = 2;

    static const uint8_t ENC_TERM_A   = 12;
    static const uint8_t ENC_TERM_B   = 3;
    static const uint8_t ENC_TERM_C   = 11;

    static const uint8_t ENC_CHANNEL  = 1;

    static const bool INVERT_OUTPUT   = true; //true for common cathode, false for common anode


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IOExpander ioe;
    Direction direction = DEFAULT_DIRECTION;
    float brightness = DEFAULT_BRIGHTNESS;
    uint interrupt_pin = PIN_UNUSED;     // A local copy of the value passed to the IOExpander, used in initialisation


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutEncoder(uint8_t address = DEFAULT_I2C_ADDRESS) : BreakoutEncoder(new I2C(), address) {};

    BreakoutEncoder(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT, bool debug = false) :
      ioe(i2c, address, interrupt, timeout, debug) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bool skip_chip_id_check = false);

    // For print access in micropython
    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    // Calls through to IOExpander class
    void set_address(uint8_t address);
    bool get_interrupt_flag();
    void clear_interrupt_flag();

    // Encoder breakout specific
    Direction get_direction();
    void set_direction(Direction direction);

    void set_brightness(float brightness);
    void set_led(uint8_t r, uint8_t g, uint8_t b);

    bool available();
    int16_t read();
    void clear();
  };

}