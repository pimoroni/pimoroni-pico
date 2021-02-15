#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

    class Trackball {
        //--------------------------------------------------
        // Constants
        //--------------------------------------------------
    public:
        static const uint8_t DEFAULT_I2C_ADDRESS        = 0x0A;
        static const uint8_t I2C_ADDRESS_ALTERNATIVE    = 0x0B;
        static const uint8_t DEFAULT_SDA_PIN            = 20;
        static const uint8_t DEFAULT_SCL_PIN            = 21;
        static const uint8_t DEFAULT_INT_PIN            = 22;
        static const uint32_t DEFAULT_TIMEOUT           = 5;
        static const uint8_t PIN_UNUSED                 = UINT8_MAX;

    private:
        static const uint16_t CHIP_ID = 0xBA11;
        static const uint8_t VERSION = 1;


        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
    private:
        i2c_inst_t *i2c     = i2c0;

        // interface pins with our standard defaults where appropriate
        int8_t address      = DEFAULT_I2C_ADDRESS;
        int8_t sda          = DEFAULT_SDA_PIN;
        int8_t scl          = DEFAULT_SCL_PIN;
        int8_t interrupt    = DEFAULT_INT_PIN;
        uint32_t timeout    = DEFAULT_TIMEOUT;


        //--------------------------------------------------
        // Constructors/Destructor
        //--------------------------------------------------
    public:
        Trackball() {};

        Trackball(uint8_t address) :
            address(address) {}

        Trackball(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT) :
            i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt), timeout(timeout) {}


        //--------------------------------------------------
        // Methods
        //--------------------------------------------------
    public: 
        bool init();

        void change_address(uint8_t new_address);
        
        void enable_interrupt(bool interrupt = true);
        bool get_interrupt();
        void set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void set_red(uint8_t value);
        void set_green(uint8_t value);
        void set_blue(uint8_t value);
        void set_white(uint8_t value);
        uint32_t read();

    private:
        uint8_t i2c_reg_read_uint8(uint8_t reg);
        void i2c_reg_write_uint8(uint8_t reg, uint8_t value);

        void wait_for_flash();

        uint32_t millis();
    };

}
