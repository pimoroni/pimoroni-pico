#include "hardware/gpio.h"
#include "wakeup.config.hpp"

extern uint32_t runtime_wakeup_gpio_state;

namespace {
    struct Wakeup {
        public:
            uint8_t shift_register_state = 0b0;

            Wakeup() {
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

#if WAKEUP_HAS_SHIFT_REGISTER==1
                // Assert shift register pins (indicator LEDs, VSYS hold etc)
                gpio_init_mask(WAKEUP_SHIFT_REG_PIN_MASK);
                gpio_set_dir_masked(WAKEUP_SHIFT_REG_PIN_MASK, WAKEUP_SHIFT_REG_PIN_DIR);
                gpio_put_masked(WAKEUP_SHIFT_REG_PIN_MASK, WAKEUP_SHIFT_REG_PIN_VALUE);

                uint8_t state = 0u;
                for(auto i = 0u; i < 8; i++) {
                    gpio_put(WAKEUP_SHIFT_REG_CLK, true);
                    if(gpio_get(WAKEUP_SHIFT_REG_DATA)) {
                        state |= (0b1 << i);
                    }
                    gpio_put(WAKEUP_SHIFT_REG_CLK, false);
                }
                shift_register_state = state;
#endif
            }
    };

    Wakeup wakeup __attribute__ ((init_priority (101)));
};

extern "C" {
#include "wakeup.h"

mp_obj_t Wakeup_get_gpio_state() {
    return mp_obj_new_int(runtime_wakeup_gpio_state);
}

mp_obj_t Wakeup_reset_gpio_state() {
    runtime_wakeup_gpio_state = 0;
    return mp_const_none;
}

void err_no_sr() {
    mp_raise_msg(&mp_type_RuntimeError, "Wakeup_get_shift_state: board does not have a shift register.");
}

mp_obj_t Wakeup_reset_shift_state() {
    #if WAKEUP_HAS_SHIFT_REGISTER==1
    wakeup.shift_register_state = 0;
    return mp_const_none;
    #endif
    err_no_sr();
    return mp_const_none;
}

mp_obj_t Wakeup_get_shift_state() {
    #if WAKEUP_HAS_SHIFT_REGISTER==1
    return mp_obj_new_int(wakeup.shift_register_state);
    #endif
    err_no_sr();
    return mp_const_none;
}

}