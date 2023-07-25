#pragma once

#include "hardware/i2c.h"

#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class TCA9555 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint DEFAULT_I2C_ADDRESS      = 0x20;
    static const int PARAM_UNUSED              = -1;
    static const uint8_t GPIO_COUNT            = 16;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  private:
    enum Registers : uint8_t {
      INPUT_PORT0         = 0x00,
      INPUT_PORT1         = 0x01,
      OUTPUT_PORT0        = 0x02,
      OUTPUT_PORT1        = 0x03,
      POLARITY_PORT0      = 0x04,
      POLARITY_PORT1      = 0x05,
      CONFIGURATION_PORT0 = 0x06,
      CONFIGURATION_PORT1 = 0x07
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // Interface pins with our standard defaults where appropriate
    uint address        = DEFAULT_I2C_ADDRESS;
    uint interrupt      = PIN_UNUSED;
    uint8_t stored_output_state[2] = {0};
    uint8_t stored_config_state[2] = {0};
    uint8_t stored_polarity_state[2] = {0};


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    //TCA9555() :
    //  TCA9555(new I2C()) {}

    TCA9555(uint address) :
      TCA9555(new I2C(), address) {}

    TCA9555(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();

    // I2C instance details access methods
    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    bool get_gpio_input(uint gpio);
    bool get_gpio_output(uint gpio);
    bool get_gpio_config(uint gpio);
    bool get_gpio_polarity(uint gpio);

    void set_gpio_output(uint gpio, bool value);
    void set_gpio_config(uint gpio, bool output);
    void set_gpio_polarity(uint gpio, bool polarity);

    uint16_t get_input_port();
    uint8_t get_input_port_low();
    uint8_t get_input_port_high();

    uint16_t get_output_port();
    uint8_t get_output_port_low();
    uint8_t get_output_port_high();

    uint16_t get_config_port();
    uint8_t get_config_port_low();
    uint8_t get_config_port_high();

    uint16_t get_polarity_port();
    uint8_t get_polarity_port_low();
    uint8_t get_polarity_port_high();

    void set_output_port(uint16_t output_state);
    void set_output_port_low(uint8_t output_state);
    void set_output_port_high(uint8_t output_state);

    void set_config_port(uint16_t config_state);
    void set_config_port_low(uint8_t config_state);
    void set_config_port_high(uint8_t config_state);

    void set_polarity_port(uint16_t polarity_state);
    void set_polarity_port_low(uint8_t polarity_state);
    void set_polarity_port_high(uint8_t polarity_state);

    void change_output_mask(uint16_t mask, uint16_t state);
  };

}