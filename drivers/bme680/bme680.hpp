#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class BME680 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x76;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x77;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;

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
    BME680() {}

    BME680(uint8_t address) :
      address(address) {}

    BME680(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    void soft_reset();
    void set_temp_offset(uint8_t value);
    void set_humidity_oversample(uint8_t value);
    uint8_t get_humidity_oversample();
    void set_pressure_oversample(uint8_t value);
    uint8_t get_pressure_oversample();
    void set_temperature_oversample(uint8_t value);
    uint8_t get_temperature_oversample();
    void set_filter(uint8_t value);
    uint8_t get_filter();
    void select_gas_heater_profile(uint8_t value);
    uint8_t get_gas_heater_profile();
    void set_gas_status(uint8_t value);
    uint8_t get_gas_status();
    void set_gas_heater_profile(uint8_t temperature, uint8_t duration, uint8_t nb_profile = 0);
    void set_gas_heater_temperature(uint8_t value, uint8_t nb_profile = 0);
    void set_gas_heater_duration(uint8_t value, uint8_t nb_profile = 0);
    void set_power_mode(uint8_t value, bool blocking = true);
    uint8_t get_power_mode();
    bool get_sensor_data();
    
  private:
    void get_calibration_data();
    void set_bits(uint8_t reg, uint8_t mask, uint8_t position, uint8_t value);
    void set_regs(uint8_t reg, uint8_t value);
    uint32_t get_regs(uint8_t reg, uint8_t length);
    uint32_t calc_temperature(uint32_t temperature_adc);
    uint32_t calc_pressure(uint32_t pressure_adc);
    uint32_t calc_humidity(uint32_t humidity_adc);
    uint32_t calc_gas_resistance(uint32_t gas_res_adc, uint32_t gas_range);
    uint16_t calc_heater_resistance(uint16_t temperature);
    uint8_t calc_heater_duration(uint16_t duration);
  };

}
