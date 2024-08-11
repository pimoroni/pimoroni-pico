#include "src/headers/MLX90640_I2C_Driver.h"
#include "mlx90640.hpp"

#include "stdio.h"


static pimoroni::I2C *i2c;

void MLX90640_I2CConfigure(pimoroni::I2C *i2c_instance) {
    i2c = i2c_instance;
}

void MLX90640_I2CInit()
{
    // i2c->init();  // Called in constructor
}

int MLX90640_I2CGeneralReset(void)
{
    return 0;
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    uint8_t cmd[2] = {(char)(startAddress >> 8), (char)(startAddress & 0xFF)};

    // Set 16-bit register pointer
    i2c->write_blocking(slaveAddr, cmd, sizeof(cmd), true);
    // Read result
    i2c->read_blocking(slaveAddr, (uint8_t*)data, nMemAddressRead * sizeof(uint16_t), false);
    
    for(auto n = 0u; n < nMemAddressRead; n++) {
        data[n] = __builtin_bswap16(data[n]);
    }

    return 0;
} 

void MLX90640_I2CFreqSet(int freq)
{
    // We can't assume we own the I2C instance and can wiggle the baudrate ad-hoc
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    uint8_t cmd[4] = {(char)(writeAddress >> 8), (char)(writeAddress & 0x00FF), (char)(data >> 8), (char)(data & 0x00FF)};
    i2c->write_blocking(slaveAddr, cmd, sizeof(cmd), false);
    return 0;
}

