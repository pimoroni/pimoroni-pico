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

    i2c_init(i2c, 100000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    uint16_t chip_id = ((uint16_t)i2c_reg_read_uint8(reg::CHIP_ID_H) << 8) | (uint16_t)i2c_reg_read_uint8(reg::CHIP_ID_L);
    if(chip_id == CHIP_ID) {
      enable_interrupt();
      succeeded = true;
    }

    return succeeded;
  }

  void Trackball::change_address(uint8_t new_address) {
    i2c_reg_write_uint8(reg::I2C_ADDR, new_address);
    wait_for_flash();
  }
  
  void Trackball::enable_interrupt(bool interrupt) {
    uint8_t value = i2c_reg_read_uint8(reg::INT);
    value &= ~MSK_INT_OUT_EN;
    if(interrupt)
      value |= MSK_INT_OUT_EN;

    i2c_reg_write_uint8(reg::INT, value);
  }
  
  bool Trackball::get_interrupt() {
    bool value = false;
    
    if(interrupt != PIN_UNUSED) {
      value = !gpio_get(interrupt);
    }
    else {
      value = i2c_reg_read_uint8(reg::INT);
      value &= MSK_INT_TRIGGERED;
    }
    return false;
  }

  void Trackball::set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    i2c_reg_write_uint8(reg::LED_RED, r);
    i2c_reg_write_uint8(reg::LED_GRN, g);
    i2c_reg_write_uint8(reg::LED_BLU, b);
    i2c_reg_write_uint8(reg::LED_WHT, w);
  }

  void Trackball::set_red(uint8_t value) {
    i2c_reg_write_uint8(reg::LED_RED, value);
  }

  void Trackball::set_green(uint8_t value) {
    i2c_reg_write_uint8(reg::LED_GRN, value);
  }

  void Trackball::set_blue(uint8_t value) {
    i2c_reg_write_uint8(reg::LED_BLU, value);
  }

  void Trackball::set_white(uint8_t value) {
    i2c_reg_write_uint8(reg::LED_WHT, value);
  }

  Trackball::State Trackball::read() {
    State state;
    uint8_t sw_state;
    state.left = i2c_reg_read_uint8(reg::LEFT);
    state.right = i2c_reg_read_uint8(reg::RIGHT);
    state.up = i2c_reg_read_uint8(reg::UP);
    state.down = i2c_reg_read_uint8(reg::DOWN);
    sw_state = i2c_reg_read_uint8(reg::SWITCH);
    
    state.sw_changed = sw_state & ~MSK_SWITCH_STATE;
    state.sw_pressed = (sw_state  & MSK_SWITCH_STATE) > 0;
    return state;
  }

  uint8_t Trackball::i2c_reg_read_uint8(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 1, false);
    return value;
  }

  void Trackball::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
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