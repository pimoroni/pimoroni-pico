#pragma once

#include <string>

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class ICP10125 {
      public:
        enum meas_command {
            NORMAL = 0x6825,
            LOW_POWER = 0x609C,
            LOW_NOISE = 0x70DF,
            ULTRA_LOW_NOISE = 0x7866,
            /*NORMAL_T_FIRST = 0x6825,
            NORMAL_P_FIRST = 0x48A3,
            LOW_POWER_T_FIRST = 0x609C,
            LOW_POWER_P_FIRST = 0x401A,
            LOW_NOISE_T_FIRST = 0x70DF,
            LOW_NOISE_P_FIRST = 0x5059,
            ULN_T_FIRST = 0x7866,
            ULN_P_FIRST = 0x58E0*/
        };

        enum reading_status {
          OK = 0,
          CRC_FAIL = 1,
        };

        struct reading {
          float temperature;
          float pressure;
          reading_status status;
        };

        static const uint8_t DEFAULT_I2C_ADDRESS = 0x63;
        static const uint8_t CHIP_ID = 0x08;

        ICP10125() : ICP10125(new I2C()) {};

        ICP10125(I2C *i2c) : i2c(i2c) {}

        bool init();
        int chip_id();
        void reset();
        reading measure(meas_command cmd=NORMAL);

      private:
        I2C *i2c;
        int8_t address = DEFAULT_I2C_ADDRESS;

        float sensor_constants[4];
        const float p_Pa_calib[3] = {45000.0f, 80000.0f, 105000.0f};
        const float LUT_lower = 3.5 * (1 << 20);
        const float LUT_upper = 11.5 * (1 << 20);
        const float quadr_factor = 1.0 / 16777216.0;
        const float offst_factor = 2048.0;
        bool read_otp();
        void process_data(const int p_LSB, const int T_LSB, float *pressure, float *temperature);
        void calculate_conversion_constants(const float *p_Pa, const float *p_LUT, float *out);
        uint8_t crc8(uint8_t *bytes, size_t length, uint8_t polynomial = 0x31);
  };

}