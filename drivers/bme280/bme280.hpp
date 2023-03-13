#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "src/bme280.h"
#include "src/bme280_defs.h"
#include "stdio.h"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {
    class BME280 {
        public:
            static const uint8_t DEFAULT_I2C_ADDRESS    = 0x76;
            static const uint8_t ALTERNATE_I2C_ADDRESS  = 0x77;
            static const uint8_t DEFAULT_INT_PIN        = I2C_DEFAULT_INT;
            
            struct i2c_intf_ptr {
                I2C *i2c;
                int8_t address;
            };

            struct bme280_reading {
                float temperature;
                float pressure;
                float humidity;
                bool status;
            };

            i2c_intf_ptr i2c_interface;

            bool debug = false;

            bool init();
            bool configure(uint8_t filter, uint8_t standby_time, uint8_t os_pressure, uint8_t os_temp, uint8_t os_humidity, uint8_t mode=BME280_NORMAL_MODE);

            BME280() : BME280(new I2C()) {}
            BME280(uint8_t address) : BME280(new I2C(), address) {}
            BME280(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED) : i2c(i2c), address(address), interrupt(interrupt) {}

            // For print access in micropython
            I2C* get_i2c() const;
            int get_int() const;

            bme280_reading read();
            BME280::bme280_reading read_forced();

            // Bindings for bme280_dev 
            static int8_t write_bytes(uint8_t reg_addr, uint8_t *reg_data, uint16_t length, void *intf_ptr) {
                BME280::i2c_intf_ptr* i2c = (BME280::i2c_intf_ptr *)intf_ptr;

                uint8_t buffer[length + 1];
                buffer[0] = reg_addr;
                for(int x = 0; x < length; x++) {
                    buffer[x + 1] = reg_data[x];
                }

                int result = i2c->i2c->write_blocking(i2c->address, buffer, length + 1, false);

                return result == PICO_ERROR_GENERIC ? BME280_E_COMM_FAIL : BME280_OK;
            };

            static int8_t read_bytes(uint8_t reg_addr, uint8_t *reg_data, uint16_t length, void *intf_ptr) {
                BME280::i2c_intf_ptr* i2c = (BME280::i2c_intf_ptr *)intf_ptr;

                int result = i2c->i2c->write_blocking(i2c->address, &reg_addr, 1, true);
                result = i2c->i2c->read_blocking(i2c->address, reg_data, length, false);

                return result == PICO_ERROR_GENERIC ? BME280_E_COMM_FAIL : BME280_OK;
            };

            static void delay_us(uint32_t period, void *intf_ptr) {
                sleep_us(period);
            }

        private:
            bme280_dev device;

            I2C *i2c;

            // interface pins with our standard defaults where appropriate
            int8_t address    = DEFAULT_I2C_ADDRESS;
            uint interrupt    = DEFAULT_INT_PIN;
    };
}