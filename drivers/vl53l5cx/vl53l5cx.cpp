#include "vl53l5cx.hpp"

namespace pimoroni {
    bool VL53L5CX::init() {
        uint8_t status = vl53l5cx_init(configuration);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::start_ranging() {
        uint8_t status = vl53l5cx_start_ranging(configuration);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::stop_ranging() {
        uint8_t status = vl53l5cx_stop_ranging(configuration);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_i2c_address(uint8_t i2c_address) {
        /* Must be a 7-bit i2c address */
        uint8_t status = vl53l5cx_set_i2c_address(configuration, i2c_address << 1);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_ranging_mode(RangingMode ranging_mode) {
        uint8_t status = vl53l5cx_set_ranging_mode(configuration, (uint8_t)ranging_mode);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_ranging_frequency_hz(uint8_t ranging_frequency_hz) {
        uint8_t status = vl53l5cx_set_ranging_frequency_hz(configuration, ranging_frequency_hz);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_resolution(Resolution resolution) {
        /* One of VL53L5CX_RESOLUTION_4X4 or VL53L5CX_RESOLUTION_8X8 */
        uint8_t status = vl53l5cx_set_resolution(configuration, (uint8_t)resolution);
        if(status == VL53L5CX_STATUS_OK) {
            this->resolution = resolution;
        }
        return status == VL53L5CX_STATUS_OK;
    }
    VL53L5CX::Resolution VL53L5CX::get_resolution() {
        //Resolution resolution = RESOLUTION_4X4;
        //vl53l5cx_get_resolution(configuration, (uint8_t *)&resolution);
        return this->resolution;
    }
    bool VL53L5CX::set_integration_time_ms(uint32_t integration_time_ms) {
        /* Integration time between 2ms and 1000ms */
        uint8_t status = vl53l5cx_set_integration_time_ms(configuration, integration_time_ms);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_sharpener_percent(uint8_t sharpener_percent) {
        /* Sharpener intensity from 0 to 99 */
        uint8_t status = vl53l5cx_set_sharpener_percent(configuration, sharpener_percent);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_target_order(TargetOrder target_order) {
        uint8_t status = vl53l5cx_set_target_order(configuration, (uint8_t)target_order);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::set_power_mode(PowerMode power_mode) {
        uint8_t status = vl53l5cx_set_power_mode(configuration, (uint8_t)power_mode);
        return status == VL53L5CX_STATUS_OK;
    }
    bool VL53L5CX::data_ready() {
        uint8_t is_ready;
        uint8_t status = vl53l5cx_check_data_ready(configuration, &is_ready);
        return status == VL53L5CX_STATUS_OK && is_ready;
    }
    bool VL53L5CX::get_data(ResultsData *results) {
        uint8_t status = vl53l5cx_get_ranging_data(configuration, results);
        return status == VL53L5CX_STATUS_OK;
    }
}