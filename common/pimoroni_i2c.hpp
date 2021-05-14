#pragma once
#include <stdint.h>
#include <climits>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pimoroni_common.hpp"
#include "pimoroni_i2c.hpp"

namespace pimoroni {
    class I2C {
      private:
        i2c_inst_t *i2c = PIMORONI_I2C_DEFAULT_INSTANCE;
        uint sda = I2C_DEFAULT_SDA;
        uint scl = I2C_DEFAULT_SCL;
        uint interrupt = PIN_UNUSED;

      public:
        I2C(BOARD board) {
          switch(board) {
            case BREAKOUT_GARDEN:
              sda = I2C_BG_SDA;
              scl = I2C_BG_SCL;
              interrupt = I2C_BG_INT;
              break;
            case PICO_EXPLORER:
            default:
              sda = I2C_DEFAULT_SDA;
              scl = I2C_DEFAULT_SCL;
              interrupt = I2C_DEFAULT_INT;
              break;
          }
          init();
        }

        I2C(uint sda, uint scl) : sda(sda), scl(scl) {
          init();
        }

        ~I2C() {
          i2c_deinit(i2c);
          gpio_disable_pulls(sda);
          gpio_set_function(sda, GPIO_FUNC_NULL);
          gpio_disable_pulls(scl);
          gpio_set_function(scl, GPIO_FUNC_NULL);
        }

        void reg_write_uint8(uint8_t address, uint8_t reg, uint8_t value);
        uint8_t reg_read_uint8(uint8_t address, uint8_t reg);
        int16_t reg_read_int16(uint8_t address, uint8_t reg);

        int write_bytes(uint8_t address, uint8_t reg, uint8_t *buf, int len);
        int read_bytes(uint8_t address, uint8_t reg, uint8_t *buf, int len);
        uint8_t get_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);
        void set_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);
        void clear_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);

        int write_blocking(uint8_t addr, const uint8_t *src, size_t len, bool nostop);
        int read_blocking(uint8_t addr, uint8_t *dst, size_t len, bool nostop);

        i2c_inst_t* get_i2c() {return i2c;}
        uint get_scl() {return scl;}
        uint get_sda() {return sda;}
      private:
        void init();
    };
}