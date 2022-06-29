#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"
#include <cstring>

constexpr char PMS5003_SOF[] = "\x42\x4d";
constexpr uint16_t PMS5003_SOFU = 0x424d;
constexpr char PMS5003_CMD_MODE_PASSIVE[] = "\xe1\x00\x00";
constexpr char PMS5003_CMD_MODE_ACTIVE[] = "\xe1\x00\x01";
constexpr char PMS5003_CMD_READ[] = "\xe2\x00\x00";
constexpr char PMS5003_CMD_SLEEP[] = "\xe4\x00\x00";
constexpr char PMS5003_CMD_WAKEUP[] = "\xe4\x00\x01";

constexpr uint PMS5003_MAX_RESET_TIME = 20000;
constexpr uint PMS5003_MAX_RESP_TIME = 5000;
constexpr uint PMS5003_MIN_CMD_INTERVAL = 100;

const uint8_t PMSA003I_DEFAULT_I2C_ADDRESS = 0x12;


class PMS5003 {
    public:

#pragma pack(push, 1)
        struct alignas(1) response_data {
            uint16_t pm_1_0;
            uint16_t pm_2_5;
            uint16_t pm_10;
            uint16_t pm_1_0_ao;
            uint16_t pm_2_5_ao;
            uint16_t pm_10_ao;
            uint16_t pm_0_3_1l;
            uint16_t pm_0_5_1l;
            uint16_t pm_1_0_1l;
            uint16_t pm_2_5_1l;
            uint16_t pm_5_1l;
            uint16_t pm_10_1l;
        };
#pragma pack(pop)

        PMS5003(uart_inst_t *uart, uint pin_tx, uint pin_rx,
                uint pin_reset, uint pin_enable)
                : uart(uart),
                pin_tx(pin_tx),
                pin_rx(pin_rx),
                pin_reset(pin_reset),
                pin_enable(pin_enable) {
                    uart_init(uart, 9600);
                    gpio_init(pin_tx);gpio_set_function(pin_tx, GPIO_FUNC_UART);
                    gpio_init(pin_rx);gpio_set_function(pin_rx, GPIO_FUNC_UART);

                    common_init();
                };

        PMS5003(pimoroni::I2C *i2c, uint pin_reset, uint pin_enable) : i2c(i2c), pin_reset(pin_reset), pin_enable(pin_enable) {
            common_init();
        }

        ~PMS5003() {};

        void reset() {
            sleep_ms(100);
            gpio_put(pin_reset, false);
            reset_input_buffer();
            sleep_ms(100);
            gpio_put(pin_reset, true);
            if(i2c) {
                sleep_ms(1000);
            }
        };

        bool read(response_data &data) {
            reset_input_buffer();

            // Read the 32 byte transaction - SOF + Size + Data + CRC
            if(i2c) {
                i2c->read_blocking(PMSA003I_DEFAULT_I2C_ADDRESS, buffer, 32, false);
            } else {
                uart_read_blocking(uart, buffer, 32);
            }

            // test the checksum matches, if not quit early with a false return value
            uint16_t checksum = (buffer[30] << 8) | buffer[31];
            uint16_t compare = 0;
            for(int i = 0; i < 30; i++) {
                compare += buffer[i];
            }

            if(compare != checksum) {
                return false;
            }
            
            // Copy the data into the result struct
            memcpy(&data, buffer + 4, sizeof(data));

            // Byteswap the results
            data.pm_1_0     = __builtin_bswap16(data.pm_1_0);
            data.pm_2_5     = __builtin_bswap16(data.pm_2_5);
            data.pm_10      = __builtin_bswap16(data.pm_10);
            data.pm_1_0_ao  = __builtin_bswap16(data.pm_1_0_ao);
            data.pm_2_5_ao  = __builtin_bswap16(data.pm_2_5_ao);
            data.pm_10_ao   = __builtin_bswap16(data.pm_10_ao);
            data.pm_0_3_1l  = __builtin_bswap16(data.pm_0_3_1l);
            data.pm_0_5_1l  = __builtin_bswap16(data.pm_0_5_1l);
            data.pm_1_0_1l  = __builtin_bswap16(data.pm_1_0_1l);
            data.pm_2_5_1l  = __builtin_bswap16(data.pm_2_5_1l);
            data.pm_5_1l    = __builtin_bswap16(data.pm_5_1l);
            data.pm_10_1l   = __builtin_bswap16(data.pm_10_1l);

            return true;
        }

    private:
        // I2C mode
        pimoroni::I2C *i2c = nullptr;

        // UART mode
        uart_inst_t *uart = nullptr;
        uint pin_tx = pimoroni::PIN_UNUSED;
        uint pin_rx = pimoroni::PIN_UNUSED;
        uint pin_reset;
        uint pin_enable;

        uint8_t buffer[64];

        void reset_input_buffer() {
            if(i2c) return;
            while(uart_is_readable(uart)) {
                uart_getc(uart);
            }
        };

        void common_init() {
            gpio_init(pin_reset);gpio_set_function(pin_reset, GPIO_FUNC_SIO);gpio_set_dir(pin_reset, GPIO_OUT);gpio_put(pin_reset, false);
            gpio_init(pin_enable);gpio_set_function(pin_enable, GPIO_FUNC_SIO);gpio_set_dir(pin_enable, GPIO_OUT);gpio_put(pin_enable, true);
            reset();
        };
};