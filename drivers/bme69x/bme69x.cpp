#include "bme69x.hpp"
#include "pico/stdlib.h"

namespace pimoroni {
    bool BME69X::init() {
        int8_t result = 0;

        if(interrupt != PIN_UNUSED) {
            gpio_set_function(interrupt, GPIO_FUNC_SIO);
            gpio_set_dir(interrupt, GPIO_IN);
            gpio_pull_up(interrupt);
        }

        i2c_interface.i2c = i2c;
        i2c_interface.address = address;

        device.intf_ptr = &i2c_interface;
        device.intf = bme69x_intf::BME69X_I2C_INTF;
        device.read = (bme69x_read_fptr_t)&read_bytes;
        device.write = (bme69x_write_fptr_t)&write_bytes;
        device.delay_us = (bme69x_delay_us_fptr_t)&delay_us;
        device.amb_temp = 20;

        result = bme69x_init(&device);
        bme69x_check_rslt("bme69x_init", result);
        if(result != BME69X_OK) return false;

        result = bme69x_get_conf(&conf, &device);
        bme69x_check_rslt("bme69x_get_conf", result);
        if(result != BME69X_OK) return false;

        configure(BME69X_FILTER_OFF, BME69X_ODR_NONE, BME69X_OS_16X, BME69X_OS_1X, BME69X_OS_2X);

        return true;
    }

    bool BME69X::configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp) {
        int8_t result = 0;

        conf.filter = filter;
        conf.odr = odr;
        conf.os_hum = os_humidity;
        conf.os_pres = os_pressure;
        conf.os_temp = os_temp;

        bme69x_set_conf(&conf, &device);
        bme69x_check_rslt("bme69x_set_conf", result);
        if(result != BME69X_OK) return false;

        return true;
    }

    bool BME69X::read_forced(bme69x_data *data, uint16_t heater_temp, uint16_t heater_duration) {
        int8_t result = 0;
        uint8_t n_fields;
        uint32_t delay_period;

        heatr_conf.enable = BME69X_ENABLE;
        heatr_conf.heatr_temp = heater_temp;
        heatr_conf.heatr_dur = heater_duration;
        result = bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatr_conf, &device);
        bme69x_check_rslt("bme69x_set_heatr_conf", result);
        if(result != BME69X_OK) return false;

        result = bme69x_set_op_mode(BME69X_FORCED_MODE, &device);
        bme69x_check_rslt("bme69x_set_op_mode", result);
        if(result != BME69X_OK) return false;

        delay_period = bme69x_get_meas_dur(BME69X_FORCED_MODE, &conf, &device) + (heatr_conf.heatr_dur * 1000);
        // Could probably just call sleep_us here directly, I guess the API uses this internally
        device.delay_us(delay_period, device.intf_ptr);

        result = bme69x_get_data(BME69X_FORCED_MODE, data, &n_fields, &device);
        bme69x_check_rslt("bme69x_get_data", result);
        if(result != BME69X_OK) return false;

        return true;
    }

    /*
    Will read profile_length results with the given temperatures and duration multipliers into the results array.
    Blocks until it has a valid result for each temp/duration, and returns the entire set in the given order.
    */
    bool BME69X::read_parallel(bme69x_data *results, uint16_t *profile_temps, uint16_t *profile_durations, size_t profile_length) {
        int8_t result;
        bme69x_data data[3]; // Parallel & Sequential mode read 3 simultaneous fields
        uint8_t n_fields;
        uint32_t delay_period;

        heatr_conf.enable = BME69X_ENABLE;
        heatr_conf.heatr_temp_prof = profile_temps;
        heatr_conf.heatr_dur_prof = profile_durations;
        heatr_conf.profile_len = profile_length;
        heatr_conf.shared_heatr_dur = 140 - (bme69x_get_meas_dur(BME69X_PARALLEL_MODE, &conf, &device) / 1000);
        result = bme69x_set_heatr_conf(BME69X_PARALLEL_MODE, &heatr_conf, &device);
        bme69x_check_rslt("bme69x_set_heatr_conf", result);
        if(result != BME69X_OK) return false;

        result = bme69x_set_op_mode(BME69X_PARALLEL_MODE, &device);
        bme69x_check_rslt("bme69x_set_op_mode", result);
        if(result != BME69X_OK) return false;

        while (1) {
            delay_period = bme69x_get_meas_dur(BME69X_PARALLEL_MODE, &conf, &device) + (heatr_conf.shared_heatr_dur * 1000);
            device.delay_us(delay_period, device.intf_ptr);

            result = bme69x_get_data(BME69X_PARALLEL_MODE, data, &n_fields, &device);
            if(result == BME69X_W_NO_NEW_DATA) continue;
            bme69x_check_rslt("bme69x_get_data", result);
            if(result != BME69X_OK) return false;

            for(auto i = 0u; i < n_fields; i++) {
                results[data[i].gas_index] = data[i];

                if(data[i].gas_index == profile_length - 1) return true;
            }
        }

        return true;
    }
}