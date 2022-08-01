#include "hardware/gpio.h"

#ifndef WAKEUP_PIN_MASK
#define WAKEUP_PIN_MASK ((0b1 << 2) | (0b1 << 6))
#endif

#ifndef WAKEUP_PIN_DIR
#define WAKEUP_PIN_DIR ((0b1 << 2) | (0b1 << 6))
#endif

#ifndef WAKEUP_PIN_VALUE
#define WAKEUP_PIN_VALUE ((0b1 << 2) | (0b1 << 6))
#endif

namespace {
    struct Wakeup {
        public:
            uint32_t wakeup_gpio_state;

            Wakeup() {
                // Assert wakeup pins (indicator LEDs, VSYS hold etc)
                gpio_init_mask(WAKEUP_PIN_MASK);
                gpio_set_dir_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_DIR);
                gpio_put_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_VALUE);

                // Latch GPIO state into variable
                Wakeup::wakeup_gpio_state = gpio_get_all();
            }
    };

    Wakeup wakeup __attribute__ ((init_priority (101)));
};

extern "C" {
#include "wakeup.h"

mp_obj_t Wakeup_get_gpio_state() {
    return mp_obj_new_int(wakeup.wakeup_gpio_state);
}

}