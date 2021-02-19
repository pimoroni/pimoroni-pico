#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class ADS1015 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x48;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x49;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;
    static const uint8_t DEFAULT_INT_PIN        = 22;
    static const uint8_t PIN_UNUSED             = UINT8_MAX;

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
    int8_t interrupt  = DEFAULT_INT_PIN;

    /***** More variables here *****/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ADS1015() {}

    ADS1015(uint8_t address) :
      address(address) {}

    ADS1015(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void start_conversion();
    bool conversion_ready();

    void set_status(uint8_t value);
    uint8_t get_status();

    void set_multiplexer(uint8_t value);
    uint8_t get_multiplexer();

    void set_mode(uint8_t value);
    uint8_t get_mode();

    void set_programmable_gain(float value = 2.048f);
    float get_programmable_gain();

    void set_sample_rate(uint16_t value);
    uint16_t get_sample_rate();

    void set_comparator_mode(uint8_t value);
    uint8_t get_comparator_mode();

    void set_comparator_latching(uint8_t value);
    uint8_t get_comparator_latching();

    void set_comparator_queue(uint8_t value);
    uint8_t get_comparator_queue();

    void wait_for_conversion(uint16_t timeout=10);

    uint8_t get_reference_voltage();
    uint8_t get_voltage(uint8_t channel/*=None*/);
    uint8_t get_compensated_voltage(uint8_t channel/*=None*/, uint32_t vdiv_a/*=8060000*/, uint32_t vdiv_b/*=402000*/, float reference_voltage/*=1.241*/);
    void get_conversion_value();

    void set_low_threshold(uint8_t value);
    uint8_t get_low_threshold();
    void set_high_threshold(uint8_t value);
    uint8_t get_high_threshold();


  private:
    /***** Private methods here *****/
  };

}
