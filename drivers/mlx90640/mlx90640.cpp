#include <stdint.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <thread>
#include <math.h>
#include "src/headers/MLX90640_API.h"
#include "mlx90640.hpp"



namespace pimoroni {
    MLX90640::MLX90640_Error MLX90640::setup(int fps){
        MLX90640_I2CConfigure(i2c_instance);
        //MLX90640_SetDeviceMode(i2c_address, 0);
        //MLX90640_SetSubPageRepeat(i2c_address, 0);

        switch(fps){
            case 1:
                MLX90640_SetRefreshRate(i2c_address, 0b001);
                break;
            case 2:
                MLX90640_SetRefreshRate(i2c_address, 0b010);
                break;
            case 4:
                MLX90640_SetRefreshRate(i2c_address, 0b011);
                break;
            case 8:
                MLX90640_SetRefreshRate(i2c_address, 0b100);
                break;
            case 16:
                MLX90640_SetRefreshRate(i2c_address, 0b101);
                if(i2c_instance->get_baudrate() < 1000000) {
                    return INVALID_BAUDRATE;
                }
                break;
            case 32:
                MLX90640_SetRefreshRate(i2c_address, 0b110);
                if(i2c_instance->get_baudrate() < 1000000) {
                    return INVALID_BAUDRATE;
                }
                break;
            case 64:
                MLX90640_SetRefreshRate(i2c_address, 0b111);
                if(i2c_instance->get_baudrate() < 1000000) {
                    return INVALID_BAUDRATE;
                }
                break;
            default:
#ifdef DEBUG
                printf("Unsupported framerate: %d", fps);
#endif
                return INVALID_FPS;
        }
        //MLX90640_SetChessMode(i2c_address);
        MLX90640_SetInterleavedMode(i2c_address);
        //MLX90640_SetResolution(i2c_address, 0);
        MLX90640_DumpEE(i2c_address, eeMLX90640);
        MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

        return OK;
    }

    int MLX90640::get_image(void){
        MLX90640_I2CConfigure(i2c_instance);

        MLX90640_GetFrameData(i2c_address, frame0);
        sleep_us(1000);
        MLX90640_GetFrameData(i2c_address, frame1);

        MLX90640_GetImage(frame0, &mlx90640, mlx90640To);
        MLX90640_GetImage(frame1, &mlx90640, mlx90640To);

        return 0;
    }

    int MLX90640::get_frame(void){
        MLX90640_I2CConfigure(i2c_instance);

        MLX90640_GetFrameData(i2c_address, frame0);
        sleep_us(1000);
        MLX90640_GetFrameData(i2c_address, frame1);

        int tr0 = MLX90640_GetTa(frame0, &mlx90640) - reflected_temperature;
        MLX90640_CalculateTo(frame0, &mlx90640, emissivity, tr0, mlx90640To);
        int tr1 = MLX90640_GetTa(frame1, &mlx90640) - reflected_temperature;
        MLX90640_CalculateTo(frame1, &mlx90640, emissivity, tr1, mlx90640To);

        return 0;
    }
}