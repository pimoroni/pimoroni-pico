#include "tca9555.hpp"

#include <chrono>
#include <cstdio>

namespace pimoroni {

  #define HIGH_BYTE(index) (((index) * 2u) + 1u)
  #define LOW_BYTE(index) (((index) * 2u))
  #define IS_PORT1(gpio) (((gpio) % TCA9555::GPIO_COUNT) >= 8u)
  #define GPIO_BYTE(gpio) ((gpio) >> 3u)
  #define GPIO_BIT_MASK(gpio) (1u << ((gpio) % 8u))
  #define CHIP_FROM_GPIO(gpio) ((gpio) / TCA9555::GPIO_COUNT)
  //#define ADDRESS_FROM_GPIO(gpio) (tca9555r_addresses[CHIP_FROM_GPIO(gpio)])

  void TCA9555::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_set_pulls(interrupt, false, true);
    }
  }

  // i2c helper methods
  i2c_inst_t* TCA9555::get_i2c() const {
    return i2c->get_i2c();
  }

  int TCA9555::get_address() const {
    return address;
  }

  int TCA9555::get_sda() const {
    return i2c->get_sda();
  }

  int TCA9555::get_scl() const {
    return i2c->get_scl();
  }

  int TCA9555::get_int() const {
    return interrupt;
  }

  bool TCA9555::get_gpio_input(uint gpio) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? INPUT_PORT1 : INPUT_PORT0;
    uint8_t input_state = i2c->reg_read_uint8(address, reg);
    return (input_state & GPIO_BIT_MASK(gpio)) != 0;
  }

  bool TCA9555::get_gpio_output(uint gpio) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    uint8_t output_state = i2c->reg_read_uint8(address, reg);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[GPIO_BYTE(gpio)] = output_state;
    #endif
    return (output_state & GPIO_BIT_MASK(gpio)) != 0;
  }

  bool TCA9555::get_gpio_config(uint gpio) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    uint8_t config_state = i2c->reg_read_uint8(address, reg);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[GPIO_BYTE(gpio)] = config_state;
    #endif
    return (config_state & GPIO_BIT_MASK(gpio)) == 0;
  }

  bool TCA9555::get_gpio_polarity(uint gpio) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    uint8_t polarity_state = i2c->reg_read_uint8(address, reg);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[GPIO_BYTE(gpio)] = polarity_state;
    #endif
    return (polarity_state & GPIO_BIT_MASK(gpio)) != 0;
  }

  void TCA9555::set_gpio_output(uint gpio, bool value) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t output_state = stored_output_state[GPIO_BYTE(tca_gpio)];
    #else
    uint8_t output_state = i2c->reg_read_uint8(address, reg);
    #endif
    uint8_t new_output_state;
    if (value) {
      new_output_state = output_state | GPIO_BIT_MASK(gpio);
    } else {
      new_output_state = output_state & ~GPIO_BIT_MASK(gpio);
    }

    if (new_output_state != output_state) {
      i2c->reg_write_uint8(address, reg, new_output_state);
      #if TCA9555R_LOCAL_MEMORY
      stored_output_state[GPIO_BYTE(gpio)] = new_output_state;
      #endif
    }
  }

  void TCA9555::set_gpio_config(uint gpio, bool output) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t config_state = tca9555r_config_state[GPIO_BYTE(gpio)];
    #else
    uint8_t config_state = i2c->reg_read_uint8(address, reg);
    #endif
    uint8_t new_config_state;
    if (output) {
      new_config_state = config_state & ~GPIO_BIT_MASK(gpio);
    } else {
      new_config_state = config_state | GPIO_BIT_MASK(gpio);
    }

    if (new_config_state != config_state) {
      i2c->reg_write_uint8(address, reg, new_config_state);
      #if TCA9555R_LOCAL_MEMORY
      stored_config_state[GPIO_BYTE(gpio)] = new_config_state;
      #endif
    }
  }

  void TCA9555::set_gpio_polarity(uint gpio, bool polarity) {
    assert(gpio < GPIO_COUNT);
    //busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    //uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t polarity_state = tca9555r_polarity_state[GPIO_BYTE(tca_gpio)];
    #else
    uint8_t polarity_state = i2c->reg_read_uint8(address, reg);
    #endif
    uint8_t new_polarity_state;
    if (polarity) {
      new_polarity_state = polarity_state | GPIO_BIT_MASK(gpio);
    } else {
      new_polarity_state = polarity_state & ~GPIO_BIT_MASK(gpio);
    }

    if (new_polarity_state != polarity_state) {
      i2c->reg_write_uint8(address, reg, new_polarity_state);
      #if TCA9555R_LOCAL_MEMORY
      tca9555r_output_state[GPIO_BYTE(gpio)] = new_polarity_state;
      #endif
    }
  }

  uint16_t TCA9555::get_input_port() {
    return i2c->reg_read_uint16(address, INPUT_PORT0);
  }

  uint8_t TCA9555::get_input_port_low() {
    return i2c->reg_read_uint8(address, INPUT_PORT0);
  }

  uint8_t TCA9555::get_input_port_high() {
    return i2c->reg_read_uint8(address, INPUT_PORT1);
  }

  uint16_t TCA9555::get_output_port() {
    uint16_t output_state = i2c->reg_read_uint16(address, OUTPUT_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    stored_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
    return output_state;
  }

  uint8_t TCA9555::get_output_port_low() {
    uint8_t output_state = i2c->reg_read_uint8(address, OUTPUT_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
  }

  uint8_t TCA9555::get_output_port_high() {
    uint8_t output_state = i2c->reg_read_uint8(address, OUTPUT_PORT1);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
  }

  uint16_t TCA9555::get_config_port() {
    uint16_t config_state = i2c->reg_read_uint16(address, CONFIGURATION_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    stored_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
    return config_state;
  }

  uint8_t TCA9555::get_config_port_low() {
    uint8_t config_state = i2c->reg_read_uint8(address, CONFIGURATION_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
  }

  uint8_t TCA9555::get_config_port_high() {
    uint8_t config_state = i2c->reg_read_uint8(address, CONFIGURATION_PORT1);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
  }

  uint16_t TCA9555::get_polarity_port() {
    uint16_t polarity_state = i2c->reg_read_uint16(address, POLARITY_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    stored_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
    return polarity_state;
  }

  uint8_t TCA9555::get_polarity_port_low() {
    uint8_t polarity_state = i2c->reg_read_uint8(address, POLARITY_PORT0);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
  }

  uint8_t TCA9555::get_polarity_port_high() {
    uint8_t polarity_state = i2c->reg_read_uint8(address, POLARITY_PORT1);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
  }

  void TCA9555::set_output_port(uint16_t output_state) {
    i2c->reg_write_uint16(address, OUTPUT_PORT0, output_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    stored_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
  }

  void TCA9555::set_output_port_low(uint8_t output_state) {
    i2c->reg_write_uint8(address, OUTPUT_PORT0, output_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
  }

  void TCA9555::set_output_port_high(uint8_t output_state) {
    i2c->reg_write_uint8(address, OUTPUT_PORT1, output_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
  }

  void TCA9555::set_config_port(uint16_t config_state) {
    i2c->reg_write_uint16(address, CONFIGURATION_PORT0, config_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    stored_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
  }

  void TCA9555::set_config_port_low(uint8_t config_state) {
    i2c->reg_write_uint8(address, CONFIGURATION_PORT0, config_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
  }

  void TCA9555::set_config_port_high(uint8_t config_state) {
    i2c->reg_write_uint8(address, CONFIGURATION_PORT1, config_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
  }

  void TCA9555::set_polarity_port(uint16_t polarity_state) {
    i2c->reg_write_uint16(address, POLARITY_PORT0, polarity_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    stored_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
  }

  void TCA9555::set_polarity_port_low(uint8_t polarity_state) {
    i2c->reg_write_uint8(address, POLARITY_PORT0, polarity_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
  }

  void TCA9555::set_polarity_port_high(uint8_t polarity_state) {
    i2c->reg_write_uint8(address, POLARITY_PORT1, polarity_state);
    #if TCA9555R_LOCAL_MEMORY
    stored_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
  }

  void TCA9555::change_output_mask(uint16_t mask, uint16_t state) {
      uint8_t low_mask = (uint8_t)(mask & 0xFF);
      uint8_t low_state = (uint8_t)(state & 0xFF);
      uint8_t high_mask = (uint8_t)(mask >> 8);
      uint8_t high_state = (uint8_t)(state >> 8);
      bool low_changed = low_mask > 0;
      bool high_changed = high_mask > 0;
      if (low_changed && high_changed) {
          #if TCA9555R_LOCAL_MEMORY
          uint16_t output_state = (stored_output_state[HIGH_BYTE(chip)] << 8) | stored_output_state[LOW_BYTE(chip)];
          #else
          uint16_t output_state = get_output_port();
          #endif
          uint16_t new_output_state = output_state;
          new_output_state &= ~mask; // Clear the mask bits
          new_output_state |= state; // Set the state bits
          if (new_output_state != output_state) {
              set_output_port(new_output_state);
          }
      } else if (low_changed) {
          #if TCA9555R_LOCAL_MEMORY
          uint8_t output_state = stored_output_state[LOW_BYTE(chip)];
          #else
          uint8_t output_state = get_output_port_low();
          #endif
          uint8_t new_output_state = (output_state & ~low_mask) | low_state;
          if (new_output_state != output_state) {
              set_output_port_low(new_output_state);
          }
      } else if (high_changed) {
          #if TCA9555R_LOCAL_MEMORY
          uint8_t output_state = tca9555r_output_state[HIGH_BYTE(chip)];
          #else
          uint8_t output_state = get_output_port_high();
          #endif
          uint8_t new_output_state = (output_state & ~high_mask) | high_state;
          if (new_output_state != output_state) {
              set_output_port_high(new_output_state);
          }
      }
  }

}
