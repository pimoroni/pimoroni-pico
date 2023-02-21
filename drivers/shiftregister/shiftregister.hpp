#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

namespace pimoroni {
    template<typename T> class ShiftRegister {
        private:
            uint CLOCK = 0;
            uint LATCH = 0;
            uint DATA = 0;

        public:
            ShiftRegister(uint clock, uint latch, uint data) :
            CLOCK(clock),
            LATCH(latch),
            DATA(data) {
                gpio_init(CLOCK);
                gpio_set_function(CLOCK, GPIO_FUNC_SIO);
                gpio_set_dir(CLOCK, GPIO_OUT);

                gpio_init(LATCH);
                gpio_set_function(LATCH, GPIO_FUNC_SIO);
                gpio_set_dir(LATCH, GPIO_OUT);

                gpio_init(DATA);
                gpio_set_function(DATA, GPIO_FUNC_SIO);
                gpio_set_dir(DATA, GPIO_IN);
            }
            T read() {
                gpio_put(LATCH, 0);
                __asm("NOP;");
                gpio_put(LATCH, 1);
                __asm("NOP;");
                T out = 0;
                for (auto i = 0u; i < sizeof(T) * 8; i++) {
                    out <<= 1;
                    out |= gpio_get(DATA);
                    gpio_put(CLOCK, 1);
                    __asm("NOP;");
                    gpio_put(CLOCK, 0);
                    __asm("NOP;");
                }
                return out;
            }
    };
}