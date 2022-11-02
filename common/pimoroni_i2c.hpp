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
        uint32_t baudrate = I2C_DEFAULT_BAUDRATE;

      public:
        I2C(BOARD board, uint32_t baudrate = I2C_DEFAULT_BAUDRATE) : baudrate(baudrate) {
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
            case PLASMA_STICK:
              sda = I2C_BG_SDA;
              scl = I2C_BG_SCL;
              interrupt = PIN_UNUSED;
              break;
            case PLASMA_2040:
            case INTERSTATE_75:
            case SERVO_2040:
            case MOTOR_2040:
              sda = I2C_HEADER_SDA;
              scl = I2C_HEADER_SCL;
              interrupt = I2C_HEADER_INT;
              break;
          }
          init();
        }

        I2C(uint sda, uint scl, uint32_t baudrate = I2C_DEFAULT_BAUDRATE) : sda(sda), scl(scl), baudrate(baudrate) {
          init();
        }

        I2C() : I2C(I2C_DEFAULT_SDA, I2C_DEFAULT_SCL) {}

        ~I2C() {
          i2c_deinit(i2c);
          gpio_disable_pulls(sda);
          gpio_set_function(sda, GPIO_FUNC_NULL);
          gpio_disable_pulls(scl);
          gpio_set_function(scl, GPIO_FUNC_NULL);
        }

        i2c_inst_t* pin_to_inst(uint pin);

        void reg_write_uint8(uint8_t address, uint8_t reg, uint8_t value);
        uint8_t reg_read_uint8(uint8_t address, uint8_t reg);
        uint16_t reg_read_uint16(uint8_t address, uint8_t reg);
        int16_t reg_read_int16(uint8_t address, uint8_t reg);
        uint32_t reg_read_uint32(uint8_t address, uint8_t reg);

        int write_bytes(uint8_t address, uint8_t reg, const uint8_t *buf, int len);
        int read_bytes(uint8_t address, uint8_t reg, uint8_t *buf, int len);
        uint8_t get_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);
        void set_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);
        void clear_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask=0b1);

        int write_blocking(uint8_t addr, const uint8_t *src, size_t len, bool nostop);
        int read_blocking(uint8_t addr, uint8_t *dst, size_t len, bool nostop);

        i2c_inst_t* get_i2c() {return i2c;}
        uint get_scl() {return scl;}
        uint get_sda() {return sda;}
        uint32_t get_baudrate() {return baudrate;}
      private:
        void init();
    };
}