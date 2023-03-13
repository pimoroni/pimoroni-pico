#include "bme280.hpp"
#include "pico/stdlib.h"

namespace pimoroni {
    bool BME280::init() {
        int8_t result;

        if(interrupt != PIN_UNUSED) {
            gpio_set_function(interrupt, GPIO_FUNC_SIO);
            gpio_set_dir(interrupt, GPIO_IN);
            gpio_pull_up(interrupt);
        }

        i2c_interface.i2c = i2c;
        i2c_interface.address = address;

        device.intf_ptr = &i2c_interface;
        device.intf = bme280_intf::BME280_I2C_INTF;
        device.read = (bme280_read_fptr_t)&read_bytes;
        device.write = (bme280_write_fptr_t)&write_bytes;
        device.delay_us = (bme280_delay_us_fptr_t)&delay_us;

        result = bme280_init(&device);
        if(result != BME280_OK) return false;

        configure(BME280_FILTER_COEFF_2, BME280_STANDBY_TIME_0_5_MS, BME280_OVERSAMPLING_16X, BME280_OVERSAMPLING_2X, BME280_OVERSAMPLING_1X);

        return true;
    }

    bool BME280::configure(uint8_t filter, uint8_t standby_time, uint8_t os_pressure, uint8_t os_temp, uint8_t os_humidity, uint8_t mode) {
        int8_t result;

        device.settings.filter = filter;
        device.settings.standby_time = standby_time;
        device.settings.osr_p = os_pressure;
        device.settings.osr_t = os_temp;
        device.settings.osr_h = os_humidity;

        result = bme280_set_sensor_settings(BME280_ALL_SETTINGS_SEL, &device);
        if(result != BME280_OK) return false;

        result = bme280_set_sensor_mode(mode, &device);
        if(result != BME280_OK) return false;

        return true;
    }

    BME280::bme280_reading BME280::read_forced() {
        bme280_reading reading;
        int8_t result;

        reading.status = false;

        result = bme280_set_sensor_mode(BME280_FORCED_MODE, &device);
        if(result != BME280_OK) return reading;

        uint32_t req_delay = bme280_cal_meas_delay(&device.settings);
        device.delay_us(req_delay, device.intf_ptr);

        bme280_data data;
        reading.status = bme280_get_sensor_data(BME280_ALL, &data, &device) == BME280_OK;
        reading.temperature = data.temperature;
        reading.pressure = data.pressure;
        reading.humidity = data.humidity;
        return reading;
    }

    BME280::bme280_reading BME280::read() {
        bme280_reading reading;
        bme280_data data;
        reading.status = bme280_get_sensor_data(BME280_ALL, &data, &device) == BME280_OK;
        reading.temperature = data.temperature;
        reading.pressure = data.pressure;
        reading.humidity = data.humidity;
        return reading;
    }

    I2C* BME280::get_i2c() const {
        return i2c;
    }

    int BME280::get_int() const {
        return interrupt;
    }
}