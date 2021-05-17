#include "trackball.hpp"
#include <cstdio>

namespace pimoroni {
  enum reg {
    LED_RED     = 0x00,
    LED_GRN     = 0x01,
    LED_BLU     = 0x02,
    LED_WHT     = 0x03,

    LEFT        = 0x04,
    RIGHT       = 0x05,
    UP          = 0x06,
    DOWN        = 0x07,
    SWITCH      = 0x08,

    USER_FLASH  = 0xD0,
    FLASH_PAGE  = 0xF0,
    INT         = 0xF9,

    CHIP_ID_L   = 0xFA,
    CHIP_ID_H   = 0xFB,
    VERSION     = 0xFC,
    I2C_ADDR    = 0xFD,
    CTRL        = 0xFE,
  };

  static const uint8_t MSK_SWITCH_STATE = 0b10000000;

  enum int_mask {
    MSK_INT_TRIGGERED = 0b00000001,
    MSK_INT_OUT_EN    = 0b00000010,
  };

  enum ctrl_mask {
    MSK_CTRL_SLEEP    = 0b00000001,
    MSK_CTRL_RESET    = 0b00000010,
    MSK_CTRL_FREAD    = 0b00000100,
    MSK_CTRL_FWRITE   = 0b00001000,
  };

  bool Trackball::init() {
    bool succeeded = false;

    if(i2c->get_baudrate() > 100000) {
      return false;
    }

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    uint16_t chip_id = ((uint16_t)i2c->reg_read_uint8(address, reg::CHIP_ID_H) << 8) | (uint16_t)i2c->reg_read_uint8(address, reg::CHIP_ID_L);
    if(chip_id == CHIP_ID) {
      enable_interrupt();
      succeeded = true;
    }

    return succeeded;
  }

  i2c_inst_t* Trackball::get_i2c() const {
    return i2c->get_i2c();
  }

  int Trackball::get_address() const {
    return address;
  }

  int Trackball::get_sda() const {
    return i2c->get_sda();
  }

  int Trackball::get_scl() const {
    return i2c->get_scl();
  }

  int Trackball::get_int() const {
    return interrupt;
  }

  void Trackball::change_address(uint8_t new_address) {
    i2c->reg_write_uint8(address, reg::I2C_ADDR, new_address);
    wait_for_flash();
  }
  
  void Trackball::enable_interrupt(bool use_interrupt) {
    uint8_t value = i2c->reg_read_uint8(address, reg::INT);
    value &= ~MSK_INT_OUT_EN;
    if(use_interrupt)
      value |= MSK_INT_OUT_EN;

    i2c->reg_write_uint8(address, reg::INT, value);
  }
  
  bool Trackball::get_interrupt() {
    bool value = false;
    
    if(interrupt != PIN_UNUSED) {
      value = !gpio_get(interrupt);
    }
    else {
      value = i2c->reg_read_uint8(address, reg::INT);
      value &= MSK_INT_TRIGGERED;
    }
    return false;
  }

  void Trackball::set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    i2c->reg_write_uint8(address, reg::LED_RED, r);
    i2c->reg_write_uint8(address, reg::LED_GRN, g);
    i2c->reg_write_uint8(address, reg::LED_BLU, b);
    i2c->reg_write_uint8(address, reg::LED_WHT, w);
  }

  void Trackball::set_red(uint8_t value) {
    i2c->reg_write_uint8(address, reg::LED_RED, value);
  }

  void Trackball::set_green(uint8_t value) {
    i2c->reg_write_uint8(address, reg::LED_GRN, value);
  }

  void Trackball::set_blue(uint8_t value) {
    i2c->reg_write_uint8(address, reg::LED_BLU, value);
  }

  void Trackball::set_white(uint8_t value) {
    i2c->reg_write_uint8(address, reg::LED_WHT, value);
  }

  Trackball::State Trackball::read() {
    State state;
    uint8_t sw_state;
    state.left = i2c->reg_read_uint8(address, reg::LEFT);
    state.right = i2c->reg_read_uint8(address, reg::RIGHT);
    state.up = i2c->reg_read_uint8(address, reg::UP);
    state.down = i2c->reg_read_uint8(address, reg::DOWN);
    sw_state = i2c->reg_read_uint8(address, reg::SWITCH);

    state.sw_changed = sw_state & ~MSK_SWITCH_STATE;
    state.sw_pressed = (sw_state  & MSK_SWITCH_STATE) > 0;
    return state;
  }

  void Trackball::wait_for_flash(void) {
    unsigned long start_time = millis();
    while(get_interrupt()) {
      if(millis() - start_time > timeout) {
        printf("Timed out waiting for interrupt!\n");
        return;
      }
      sleep_ms(1);
    }

    start_time = millis();
    while(!get_interrupt()) {
      if(millis() - start_time > timeout) {
        printf("Timed out waiting for interrupt!\n");
        return;
      }
      sleep_ms(1);
    }
  }

  uint32_t Trackball::millis() {
    return to_ms_since_boot(get_absolute_time());
  }
}