#include <stdint.h>
#include "src/headers/MLX90640_API.h"
#include "common/pimoroni_i2c.hpp"

void MLX90640_I2CConfigure(pimoroni::I2C *i2c_instance);

#define MLX90640_DEFAULT_I2C_ADDRESS 0x33

namespace pimoroni {
    class MLX90640 {
        public:
            static const int WIDTH = 32;
            static const int HEIGHT = 24;

            enum MLX90640_Error {
                OK = 0,
                INVALID_BAUDRATE = 1,
                INVALID_FPS = 2,
            };

            float mlx90640To[WIDTH * HEIGHT] = {0.0f};
            float emissivity = 1.0f;
            float reflected_temperature = 8.0f;

            MLX90640(pimoroni::I2C *i2c_instance, uint i2c_address=MLX90640_DEFAULT_I2C_ADDRESS) : i2c_instance(i2c_instance), i2c_address(i2c_address) {};
            MLX90640_Error setup(int fps);
            int get_image(void);
            int get_frame(void);
        private:
            pimoroni::I2C *i2c_instance;
            uint i2c_address = MLX90640_DEFAULT_I2C_ADDRESS;
            paramsMLX90640 mlx90640;
            uint16_t eeMLX90640[832] = {0};
            uint16_t frame0[834] = {0};
            uint16_t frame1[834] = {0};
    };
}