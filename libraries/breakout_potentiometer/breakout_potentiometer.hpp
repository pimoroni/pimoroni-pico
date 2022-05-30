#pragma once

#include "drivers/ioexpander/ioexpander.hpp"

namespace pimoroni {

  class BreakoutPotentiometer {
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
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x0E;
    static constexpr float DEFAULT_BRIGHTNESS = 1.0f; //Effectively the maximum fraction of the period that the LED will be on
    static const Direction DEFAULT_DIRECTION  = DIRECTION_CW;
    static const uint32_t DEFAULT_TIMEOUT     = 1;
    static const uint32_t DEFAULT_ADC_TIMEOUT = 1;

  private:
    static const uint8_t LED_R        = 1;
    static const uint8_t LED_G        = 7;
    static const uint8_t LED_B        = 2;

    static const uint8_t POT_TERM_A   = 12;
    static const uint8_t POT_TERM_B   = 3;
    static const uint8_t POT_INPUT    = 11;

    static const bool INVERT_OUTPUT   = true; //true for common cathode, false for common anode


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IOExpander ioe;
    Direction direction = DEFAULT_DIRECTION;
    float brightness = DEFAULT_BRIGHTNESS;

    
    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutPotentiometer(uint8_t address = DEFAULT_I2C_ADDRESS) : BreakoutPotentiometer(new I2C(), address) {};

    BreakoutPotentiometer(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT, bool debug = false) :
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

    float get_adc_vref();
    void set_adc_vref(float vref);

    // Potentiometer breakout specific
    Direction get_direction();
    void set_direction(Direction direction);

    void set_brightness(float brightness);
    void set_led(uint8_t r, uint8_t g, uint8_t b);

    float read(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    int16_t read_raw(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
  };

}