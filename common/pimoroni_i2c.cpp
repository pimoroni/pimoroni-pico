#include "pimoroni_common.hpp"
#include "pimoroni_i2c.hpp"

namespace pimoroni {
    void I2C::init() {
        i2c = pin_to_inst(sda);
        // TODO call pin_to_inst on sda and scl, and verify they are a valid i2c pin pair
        // TODO maybe also fall back to PIO i2c for non-standard pin combinations

        // Since it's easy to leave the I2C in a bad state when experimenting in the MicroPython REPL
        // this loop will find any I2C pins relevant to the current instance and reset them.
        for(auto pin = 0u; pin < 30; pin++) {
            if(pin_to_inst(pin) == i2c && gpio_get_function(pin) == GPIO_FUNC_I2C) {
                gpio_disable_pulls(pin);
                gpio_set_function(pin, GPIO_FUNC_NULL);
            }
        }

        i2c_init(i2c, baudrate);
        gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
        gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);
    }

    i2c_inst_t* I2C::pin_to_inst(uint pin) {
        return ((pin >> 1) & 0b1) ? i2c1 : i2c0;
    }

    /* Basic wrappers for devices using i2c functions directly */
    int I2C::write_blocking(uint8_t addr, const uint8_t *src, size_t len, bool nostop) {
        return i2c_write_blocking(i2c, addr, src, len, nostop);
    }

    int I2C::read_blocking(uint8_t addr, uint8_t *dst, size_t len, bool nostop) {
        return i2c_read_blocking(i2c, addr, dst, len, nostop);
    }

    /* Convenience functions for various common i2c operations */
    void I2C::reg_write_uint8(uint8_t address, uint8_t reg, uint8_t value) {
        uint8_t buffer[2] = {reg, value};
        i2c_write_blocking(i2c, address, buffer, 2, false);
    }

    uint8_t I2C::reg_read_uint8(uint8_t address, uint8_t reg) {
        uint8_t value;
        i2c_write_blocking(i2c, address, &reg, 1, false);
        i2c_read_blocking(i2c, address, (uint8_t *)&value, sizeof(uint8_t), false);
        return value;
    }

    uint16_t I2C::reg_read_uint16(uint8_t address, uint8_t reg) {
        uint16_t value;
        i2c_write_blocking(i2c, address, &reg, 1, true);
        i2c_read_blocking(i2c, address, (uint8_t *)&value, sizeof(uint16_t), false);
        return value;
    }

    uint32_t I2C::reg_read_uint32(uint8_t address, uint8_t reg) {
        uint32_t value;
        i2c_write_blocking(i2c, address, &reg, 1, true);
        i2c_read_blocking(i2c, address, (uint8_t *)&value, sizeof(uint32_t), false);
        return value;
    }

    int16_t I2C::reg_read_int16(uint8_t address, uint8_t reg) {
        int16_t value;
        i2c_write_blocking(i2c, address, &reg, 1, true);
        i2c_read_blocking(i2c, address, (uint8_t *)&value, sizeof(int16_t), false);
        return value;
    }

    int I2C::write_bytes(uint8_t address, uint8_t reg, const uint8_t *buf, int len) {
        uint8_t buffer[len + 1];
        buffer[0] = reg;
        for(int x = 0; x < len; x++) {
            buffer[x + 1] = buf[x];
        }
        return i2c_write_blocking(i2c, address, buffer, len + 1, false);
    };

    int I2C::read_bytes(uint8_t address, uint8_t reg, uint8_t *buf, int len) {
        i2c_write_blocking(i2c, address, &reg, 1, true);
        i2c_read_blocking(i2c, address, buf, len, false);
        return len;
    };

    uint8_t I2C::get_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask) {
        uint8_t value;
        read_bytes(address, reg, &value, 1);
        return value & (mask << shift);
    }

    void I2C::set_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask) {
        uint8_t value;
        read_bytes(address, reg, &value, 1);
        value |= mask << shift;
        write_bytes(address, reg, &value, 1);
    }

    void I2C::clear_bits(uint8_t address, uint8_t reg, uint8_t shift, uint8_t mask) {
        uint8_t value;
        read_bytes(address, reg, &value, 1);
        value &= ~(mask << shift);
        write_bytes(address, reg, &value, 1);
    }
}