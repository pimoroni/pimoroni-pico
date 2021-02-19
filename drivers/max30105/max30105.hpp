#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class MAX30105 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x57;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;
    static const uint8_t DEFAULT_INT_PIN      = 22;
    static const uint8_t PIN_UNUSED           = UINT8_MAX;

    /***** More public constants here *****/
    static const uint8_t CHIP_ID    = 0x15;

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
    MAX30105() {}

    MAX30105(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    /***** More public methods here *****/

    void setup(float led_power = 6.4, uint8_t sample_average = 4, uint8_t leds_enable = 3,
      uint16_t sample_rate = 400, uint16_t pulse_width = 215, uint16_t adc_range = 16384, float timeout = 5.0f);

    void soft_reset(float timeout = 5.0f);
    void clear_fifo();
    uint8_t get_samples();
    uint8_t get_chip_id();
    uint8_t get_temperature(float timeout = 5.0f);
    void set_mode(uint8_t mode);
    void set_slot_mode(uint8_t slot, uint8_t mode);
    void set_led_pulse_amplitude(uint8_t led, uint8_t amplitude);
    void set_fifo_almost_full_count(uint8_t count);
    void set_fifo_almost_full_enable(uint8_t value);
    void set_data_ready_enable(uint8_t value);
    void set_ambient_light_compensation_overflow_enable(uint8_t value);
    void set_proximity_enable(uint8_t value);
    void set_proximity_threshold(uint8_t value);
    uint8_t get_fifo_almost_full_status();
    uint8_t get_data_ready_status();
    uint8_t get_ambient_light_compensation_overflow_status();
    uint8_t get_proximity_triggered_threshold_status();
    uint8_t get_power_ready_status();
    uint8_t get_die_temp_ready_status();

  private:
    /***** Private methods here *****/
  };

}
