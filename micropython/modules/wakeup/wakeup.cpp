#include "hardware/gpio.h"
#include "wakeup.config.hpp"


const uint32_t wakeup_magic = 0x91C0;
__attribute__((section(".uninitialized_bss"))) static uint32_t runtime_wakeup_reason[2];
extern uint32_t runtime_wakeup_gpio_state;

namespace {
    struct Wakeup {
        public:
            Wakeup() {

                if(runtime_wakeup_reason[0] != wakeup_magic) {
                    runtime_wakeup_reason[0] = wakeup_magic;
                    runtime_wakeup_reason[1] = 0;
                }

                // Assert wakeup pins (indicator LEDs, VSYS hold etc)
                //gpio_init_mask(WAKEUP_PIN_MASK);
                //gpio_set_dir_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_DIR);
                //gpio_put_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_VALUE);

#if WAKEUP_HAS_RTC==1
                // Set up RTC I2C pins and send reset command
                i2c_init(WAKEUP_RTC_I2C_INST, 100000);
                gpio_init(WAKEUP_RTC_SDA);
                gpio_init(WAKEUP_RTC_SCL);
                gpio_set_function(WAKEUP_RTC_SDA, GPIO_FUNC_I2C); gpio_pull_up(WAKEUP_RTC_SDA);
                gpio_set_function(WAKEUP_RTC_SCL, GPIO_FUNC_I2C); gpio_pull_up(WAKEUP_RTC_SCL);

                // Turn off CLOCK_OUT by writing 0b111 to CONTROL_2 (0x01) register
                uint8_t data[] = {0x01, 0b111};
                i2c_write_blocking(WAKEUP_RTC_I2C_INST, WAKEUP_RTC_I2C_ADDR, data, 2, false);

                i2c_deinit(WAKEUP_RTC_I2C_INST);

                // Cleanup
                gpio_init(WAKEUP_RTC_SDA);
                gpio_init(WAKEUP_RTC_SCL);
#endif
            }
    };

    Wakeup wakeup __attribute__ ((init_priority (101)));
};

extern "C" {
#include "wakeup.h"

mp_obj_t Wakeup_get_wake_reason() {
    return mp_obj_new_int(runtime_wakeup_reason[1]);
}

mp_obj_t Wakeup_set_wake_reason(mp_obj_t value) {
    runtime_wakeup_reason[1] = mp_obj_get_int(value);
    return mp_const_none;
}

mp_obj_t Wakeup_get_gpio_state() {
    return mp_obj_new_int(runtime_wakeup_gpio_state);
}

}