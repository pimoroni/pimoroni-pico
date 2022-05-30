#pragma once

#include "drivers/ioexpander/ioexpander.hpp"

namespace pimoroni {

  class BreakoutMICS6814 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x19;
    static constexpr float DEFAULT_BRIGHTNESS = 1.0f; //Effectively the maximum fraction of the period that the LED will be on
    static const uint32_t DEFAULT_TIMEOUT     = 1;
    static const uint32_t DEFAULT_ADC_TIMEOUT = 1;

  private:
    static const uint8_t LED_R            = 3;
    static const uint8_t LED_G            = 7;
    static const uint8_t LED_B            = 2;

    static const uint8_t MICS_VREF        = 14;
    static const uint8_t MICS_RED         = 12;
    static const uint8_t MICS_NH3         = 11;
    static const uint8_t MICS_OX          = 13;
    static const uint8_t MICS_HEATER_EN   = 1;

    static const bool INVERT_OUTPUT   = true; //true for common cathode, false for common anode


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct Reading {
      float ref;
      float reducing;
      float nh3;
      float oxidising;
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IOExpander ioe;
    float brightness = DEFAULT_BRIGHTNESS;

    
    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutMICS6814(uint8_t address = DEFAULT_I2C_ADDRESS) : BreakoutMICS6814(new I2C(), address) {};

    BreakoutMICS6814(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT, bool debug = false) :
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

    // MICS breakout specific
    void set_brightness(float brightness);
    void set_led(uint8_t r, uint8_t g, uint8_t b);

    void set_heater(bool on);
    void disable_heater();

    float get_raw_ref(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float get_raw_red(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float get_raw_nh3(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float get_raw_oxd(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);

    Reading read_all(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float read_ref(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float read_reducing(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float read_nh3(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
    float read_oxidising(uint32_t adc_timeout = DEFAULT_ADC_TIMEOUT);
  };

}