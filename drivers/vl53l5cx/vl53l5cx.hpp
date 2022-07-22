#pragma once

extern "C" {
#include "drivers/vl53l5cx/src/VL53L5CX_ULD_API/inc/vl53l5cx_api.h"
#include "drivers/vl53l5cx/src/VL53L5CX_ULD_API/inc/vl53l5cx_plugin_motion_indicator.h"
}

#include <new>
#include "common/pimoroni_i2c.hpp"
#include "src/VL53L5CX_ULD_API/inc/vl53l5cx_api.h"
#include "src/VL53L5CX_ULD_API/inc/vl53l5cx_plugin_motion_indicator.h"

namespace pimoroni {
    class VL53L5CX {
        public:
            typedef VL53L5CX_ResultsData ResultsData;
            enum TargetOrder : uint8_t {
                TARGET_ORDER_CLOSEST = VL53L5CX_TARGET_ORDER_CLOSEST,
                TARGET_ORDER_STRONGEST = VL53L5CX_TARGET_ORDER_STRONGEST
            };
            enum Resolution : uint8_t {
                RESOLUTION_4X4 = VL53L5CX_RESOLUTION_4X4,
                RESOLUTION_8X8 = VL53L5CX_RESOLUTION_8X8
            };
            enum RangingMode : uint8_t {
                RANGING_MODE_CONTINUOUS = VL53L5CX_RANGING_MODE_CONTINUOUS,
                RANGING_MODE_AUTONOMOUS = VL53L5CX_RANGING_MODE_AUTONOMOUS
            };
            enum PowerMode : uint8_t {
                POWER_MODE_SLEEP = VL53L5CX_POWER_MODE_SLEEP,
                POWER_MODE_WAKEUP = VL53L5CX_POWER_MODE_WAKEUP
            };

            // 7-bit version of the default address (0x52)
            static const uint8_t DEFAULT_ADDRESS = VL53L5CX_DEFAULT_I2C_ADDRESS >> 1;

            VL53L5CX(I2C *i2c, uint8_t *firmware, uint8_t i2c_addr=DEFAULT_ADDRESS, void* configuration_buffer=nullptr, void* motion_configuration_buffer=nullptr) {
                if(!configuration_buffer) configuration_buffer = new VL53L5CX_Configuration;
                if(!motion_configuration_buffer) motion_configuration_buffer = new VL53L5CX_Motion_Configuration;

                configuration = new(configuration_buffer) VL53L5CX_Configuration{
                    .platform = VL53L5CX_Platform{
                        .address = i2c_addr,
                        .i2c = i2c->get_i2c(),
                        .firmware = firmware
                    },
                };
                motion_configuration = new(motion_configuration_buffer) VL53L5CX_Motion_Configuration{};
            }
            ~VL53L5CX() {
                delete configuration;
                delete motion_configuration;
            }
            bool init();
            bool is_alive();
            bool start_ranging();
            bool stop_ranging();

            bool enable_motion_indicator(Resolution resolution);
            bool set_motion_distance(uint16_t distance_min, uint16_t distance_max);

            bool set_i2c_address(uint8_t i2c_address);
            bool set_ranging_mode(RangingMode ranging_mode);
            bool set_ranging_frequency_hz(uint8_t ranging_frequency_hz);

            bool set_resolution(Resolution resolution);
            Resolution get_resolution();

            bool set_integration_time_ms(uint32_t integration_time_ms);
            bool set_sharpener_percent(uint8_t sharpener_percent);
            bool set_target_order(TargetOrder target_order);
            bool set_power_mode(PowerMode power_mode);
            bool data_ready();
            bool get_data(ResultsData *results);

            VL53L5CX_Configuration* get_configuration() {
                return configuration;
            }
        private:
            VL53L5CX_Configuration *configuration;
            VL53L5CX_Motion_Configuration *motion_configuration;
            Resolution resolution = RESOLUTION_8X8;
    };
}