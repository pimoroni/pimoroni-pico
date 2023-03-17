#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "bme68x.h"
#include "bme68x_defs.h"
#include "common/pimoroni_i2c.hpp"
#include "stdio.h"

namespace pimoroni {
    class BME68X {
        public:
            static const uint8_t DEFAULT_I2C_ADDRESS    = 0x76;
            static const uint8_t ALTERNATE_I2C_ADDRESS  = 0x77;
            
            struct i2c_intf_ptr {
                I2C *i2c;
                int8_t address;
            };

            i2c_intf_ptr i2c_interface;

            bool debug = true;

            bool init();
            bool configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp);
            bool read_forced(bme68x_data *data, uint16_t heater_temp=300, uint16_t heater_duration=100);
            bool read_parallel(bme68x_data *results, uint16_t *profile_temps, uint16_t *profile_durations, size_t profile_length);

            BME68X() : BME68X(new I2C()) {}
            BME68X(uint8_t address, uint interrupt = PIN_UNUSED) : BME68X(new I2C(), address, interrupt) {}
            BME68X(I2C *i2c, uint8_t address = DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED) : i2c(i2c), address(address), interrupt(interrupt) {}

            I2C *get_i2c() {return i2c;}
            uint get_int() {return PIN_UNUSED;}

            // Bindings for bme68x_dev 
            static int write_bytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr) {
                BME68X::i2c_intf_ptr* i2c = (BME68X::i2c_intf_ptr *)intf_ptr;

                uint8_t buffer[length + 1];
                buffer[0] = reg_addr;
                for(auto x = 0u; x < length; x++) {
                    buffer[x + 1] = reg_data[x];
                }

                int result = i2c->i2c->write_blocking(i2c->address, buffer, length + 1, false);

                return result == PICO_ERROR_GENERIC ? 1 : 0;
            };

            static int read_bytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr) {
                BME68X::i2c_intf_ptr* i2c = (BME68X::i2c_intf_ptr *)intf_ptr;

                int result = i2c->i2c->write_blocking(i2c->address, &reg_addr, 1, true);
                result = i2c->i2c->read_blocking(i2c->address, reg_data, length, false);

                return result == PICO_ERROR_GENERIC ? 1 : 0;
            };

            static void delay_us(uint32_t period, void *intf_ptr) {
                sleep_us(period);
            }

            /* From BME68X API examples/common/common.c */
            void bme68x_check_rslt(const char api_name[], int8_t rslt)
            {
                if(!debug) return;
                switch (rslt)
                {
                    case BME68X_OK:
                        /* Do nothing */
                        break;
                    case BME68X_E_NULL_PTR:
                        printf("%s: Error [%d] : Null pointer\r\n", api_name, rslt);
                        break;
                    case BME68X_E_COM_FAIL:
                        printf("%s: Error [%d] : Communication failure\r\n", api_name, rslt);
                        break;
                    case BME68X_E_INVALID_LENGTH:
                        printf("%s: Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
                        break;
                    case BME68X_E_DEV_NOT_FOUND:
                        printf("%s: Error [%d] : Device not found\r\n", api_name, rslt);
                        break;
                    case BME68X_E_SELF_TEST:
                        printf("%s: Error [%d] : Self test error\r\n", api_name, rslt);
                        break;
                    case BME68X_W_NO_NEW_DATA:
                        printf("%s: Warning [%d] : No new data found\r\n", api_name, rslt);
                        break;
                    default:
                        printf("%s: Error [%d] : Unknown error code\r\n", api_name, rslt);
                        break;
                }
            }

        private:
            bme68x_dev device;
            bme68x_conf conf;
            bme68x_heatr_conf heatr_conf;

            I2C *i2c;

            int8_t address    = DEFAULT_I2C_ADDRESS;
            uint interrupt    = I2C_DEFAULT_INT;
    };
}