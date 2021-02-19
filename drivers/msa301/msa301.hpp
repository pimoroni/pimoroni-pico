#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class MSA301 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t I2C_ADDRESS      = 0x26;
    static const uint8_t DEFAULT_SDA_PIN  = 20;
    static const uint8_t DEFAULT_SCL_PIN  = 21;
    static const uint8_t DEFAULT_INT_PIN  = 22;
    static const uint8_t PIN_UNUSED       = UINT8_MAX;

    static const uint8_t X = 0x02;
    static const uint8_t Y = 0x04;
    static const uint8_t Z = 0x06;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c  = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = I2C_ADDRESS;
    int8_t sda       = DEFAULT_SDA_PIN;
    int8_t scl       = DEFAULT_SCL_PIN;
    int8_t interrupt = DEFAULT_INT_PIN;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MSA301() {}

    MSA301(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    float get_axis(uint8_t axis, uint8_t sample_count = 1);

    uint8_t twos_comp_conversion(uint8_t val, uint8_t bits = 14);
    uint8_t twos_comp_to_bin(uint8_t val);
    uint8_t get_part_id();
    void set_range(uint8_t value);
    void set_resolution(uint8_t resolution);
    uint8_t get_current_range_and_resolution();
    void set_output_data_rate(uint8_t rate);
    uint8_t get_output_data_rate();
    uint8_t get_power_mode();
    void set_power_mode(uint8_t mode);
    uint8_t get_interrupt();
    void enable_interrupt(uint8_t interrupts);
    uint8_t wait_for_interrupt(uint8_t interrupts, float polling_delay = 0.01f);
    void disable_all_interrupts();
    uint8_t get_raw_measurements();
    uint8_t get_raw_offsets();
    void set_raw_offsets(uint8_t offset_x = 0, uint8_t offset_y = 0, uint8_t offset_z = 0);
    uint8_t get_offsets();
    void set_offsets(uint8_t offset_x = 0, uint8_t offset_y = 0, uint8_t offset_z = 0);

  private:
    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    int16_t i2c_reg_read_int16(uint8_t reg);
  };

}
