#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>
#include <cstring>

#include "icp10125.hpp"

namespace pimoroni {

    enum command {
        SOFT_RESET = 0x805D,
        READ_ID = 0xEFC8,
        MOVE_ADDRESS_PTR = 0xC595,
        READ_OTP = 0xC7F7
    };
    
#pragma pack(push, 1)
    struct alignas(1) uint16_result {
        uint16_t data;
        uint8_t crc8;
    };
#pragma pack(pop)

    struct conversion_constants {
        float A;
        float B;
        float C;
    };

    bool ICP10125::init() {
        reset();
        uint8_t id = chip_id();
        if(id != CHIP_ID) return false;

        if(!read_otp()) return false;

        return true;
    }

    void ICP10125::reset() {
        uint16_t command = __builtin_bswap16(SOFT_RESET);
        i2c->write_blocking(address, (uint8_t *)&command, 2, false);
        sleep_ms(10); // Soft reset time is 170us but you can never be too sure...
    }

    ICP10125::reading ICP10125::measure(meas_command cmd) {
        uint16_t command = __builtin_bswap16(cmd);
        reading result = {0.0f, 0.0f, OK};
        uint16_result results[3];

        i2c->write_blocking(address, (uint8_t *)&command, 2, false);

        switch(cmd) {
            case NORMAL:
                sleep_ms(7); // 5.6 - 6.3ms
                break;
            case LOW_POWER:
                sleep_ms(2); // 1.6 - 1.8ms
                break;
            case LOW_NOISE:
                sleep_ms(24); // 20.8 - 23.8ms
                break;
            case ULTRA_LOW_NOISE:
                sleep_ms(95); // 83.2 - 94.5ms
                break;
        }

        // Can probably just run this until it succeeds rather than the switch/sleep above.
        // The datasheet implies polling and ignoring NACKs would work.
        i2c->read_blocking(address, (uint8_t *)&results, 9, false);

        if(results[0].crc8 != crc8((uint8_t *)&results[0].data, 2)) {result.status = CRC_FAIL; return result;};
        if(results[1].crc8 != crc8((uint8_t *)&results[1].data, 2)) {result.status = CRC_FAIL; return result;};
        if(results[2].crc8 != crc8((uint8_t *)&results[2].data, 2)) {result.status = CRC_FAIL; return result;};

        int temperature = __builtin_bswap16(results[0].data);
        // Due to all the byte swapping nonsense I'm not sure if I've discarded the LLSB or LMSB here...
        int pressure = ((int32_t)__builtin_bswap16(results[1].data) << 8) | (__builtin_bswap16(results[2].data >> 8)); // LLSB is discarded

        process_data(pressure, temperature, &result.pressure, &result.temperature);
        return result;
    }

    int ICP10125::chip_id() {
        uint16_result result;
        uint16_t command = __builtin_bswap16(READ_ID);

        i2c->write_blocking(address, (uint8_t *)&command, 2, true);
        i2c->read_blocking(address, (uint8_t *)&result, 3, false);

        if(result.crc8 != crc8((uint8_t *)&result.data, 2)) {
            return -1;
        }

        return __builtin_bswap16(result.data) & 0x3f;
    }

    bool ICP10125::read_otp() {
        uint16_result result[4];
        uint16_t command = __builtin_bswap16(READ_OTP);
        uint8_t move_address_ptr[] = {
            MOVE_ADDRESS_PTR >> 8, MOVE_ADDRESS_PTR & 0xff,
            0x00,
            0x66,
            0x9c  // Address CRC8
        };

        i2c->write_blocking(address, move_address_ptr, sizeof(move_address_ptr), false);

        for(auto x = 0u; x < 4; x++) {
            i2c->write_blocking(address, (uint8_t *)&command, 2, false);
            i2c->read_blocking(address, (uint8_t *)&result[x], 3, false);
            if(result[x].crc8 != crc8((uint8_t *)&result[x].data, 2)) {
                return false;
            }
            sensor_constants[x] = (float)__builtin_bswap16(result[x].data);
        }

        return true;
    }

    void ICP10125::process_data(const int p_LSB, const int T_LSB, float *pressure, float *temperature) { 
        float t; 
        float s1, s2, s3; 
        float in[3]; 
        float out[3]; 
        float A, B, C; 
        
        t = (float)(T_LSB - 32768); 
        s1 = LUT_lower + (float)(sensor_constants[0] * t * t) * quadr_factor; 
        s2 = offst_factor * sensor_constants[3] + (float)(sensor_constants[1] * t * t) * quadr_factor; 
        s3 = LUT_upper + (float)(sensor_constants[2] * t * t) * quadr_factor; 
        in[0] = s1; 
        in[1] = s2; 
        in[2] = s3; 
        
        calculate_conversion_constants(p_Pa_calib, in, out); 
        A = out[0]; 
        B = out[1]; 
        C = out[2]; 
        
        *pressure = A + B / (C + p_LSB); 
        *temperature = -45.f + 175.f / 65536.f * T_LSB;
    }

    void ICP10125::calculate_conversion_constants(const float *p_Pa, const float *p_LUT, float *out) { 
        float A, B, C; 
        
        C = (p_LUT[0] * p_LUT[1] * (p_Pa[0] - p_Pa[1]) + 
        p_LUT[1] * p_LUT[2] * (p_Pa[1] - p_Pa[2]) + 
        p_LUT[2] * p_LUT[0] * (p_Pa[2] - p_Pa[0])) / 
        (p_LUT[2] * (p_Pa[0] - p_Pa[1]) + 
        p_LUT[0] * (p_Pa[1] - p_Pa[2]) + 
        p_LUT[1] * (p_Pa[2] - p_Pa[0])); 
        A = (p_Pa[0] * p_LUT[0] - p_Pa[1] * p_LUT[1] - (p_Pa[1] - p_Pa[0]) * C) / (p_LUT[0] - p_LUT[1]); 
        B = (p_Pa[0] - A) * (p_LUT[0] + C); 
        
        out[0] = A; 
        out[1] = B; 
        out[2] = C; 
    }

    uint8_t ICP10125::crc8(uint8_t *bytes, size_t length, uint8_t polynomial) {
        uint8_t result = 0xff;
        for (auto byte = 0u; byte < length; byte++) {
            result ^= bytes[byte];
            for (auto bit = 0u; bit < 8; bit++) {
                if (result & 0x80) {
                    result <<= 1;
                    result ^= polynomial;
                } else {
                    result <<= 1;
                }
            }
        }
        return result;
    }

}