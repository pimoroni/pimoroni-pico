#include "bmp280.hpp"
#include "pico/stdlib.h"

namespace pimoroni {
    bool BMP280::init() {
        int8_t result;

        if(interrupt != PIN_UNUSED) {
            gpio_set_function(interrupt, GPIO_FUNC_SIO);
            gpio_set_dir(interrupt, GPIO_IN);
            gpio_pull_up(interrupt);
        }

        i2c_interface.i2c = i2c;
        i2c_interface.address = address;

        device.intf_ptr = &i2c_interface;
        device.intf = BMP280_I2C_INTF;
        device.read = (bmp280_com_fptr_t)&read_bytes;
        device.write = (bmp280_com_fptr_t)&write_bytes;
        device.delay_ms = (bmp280_delay_fptr_t)&delay_ms;

        result = bmp280_init(&device);
        if(result != BMP280_OK) return false;

        result = bmp280_set_config(&conf, &device);
        if(result != BMP280_OK) return false;

        configure(BMP280_FILTER_COEFF_2, BMP280_ODR_1000_MS, BMP280_OS_4X, BMP280_OS_4X);

        return true;
    }

    bool BMP280::configure(uint8_t filter, uint8_t odr, uint8_t os_pressure, uint8_t os_temp, uint8_t mode) {
        int8_t result;

        conf.filter = filter;
        conf.odr = odr;
        conf.os_pres = os_pressure;
        conf.os_temp = os_temp;

        result = bmp280_set_config(&conf, &device);
        if(result != BMP280_OK) return false;

        result = bmp280_set_power_mode(mode, &device);
        if(result != BMP280_OK) return false;

        return true;
    }

    BMP280::bmp280_reading BMP280::read() {
        bmp280_reading result;
        bmp280_get_uncomp_data(&ucomp_data, &device);

        int32_t temperature;
        result.status = bmp280_get_comp_temp_32bit(&temperature, ucomp_data.uncomp_temp, &device);
        result.temperature = 0.01f * temperature;

        result.status &= bmp280_get_comp_pres_32bit(&result.pressure, ucomp_data.uncomp_press, &device);
        
        return result;
    }

    I2C* BMP280::get_i2c() const {
        return i2c;
    }

    int BMP280::get_int() const {
        return interrupt;
    }
}