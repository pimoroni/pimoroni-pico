#include "wakeup.h"
#include "hardware/gpio.h"
#include "wakeup.config.hpp"
#include "pico/runtime_init.h"


uint32_t runtime_wakeup_gpio_state = 0;

// Pins to toggle on wakeup
#ifndef PICO_WAKEUP_PIN_MASK
#define PICO_WAKEUP_PIN_MASK ((0b1 << 2) | (0b1 << 6))
#endif

// Direction
#ifndef PICO_WAKEUP_PIN_DIR
#define PICO_WAKEUP_PIN_DIR ((0b1 << 2) | (0b1 << 6))
#endif

// Value
#ifndef PICO_WAKEUP_PIN_VALUE
#define PICO_WAKEUP_PIN_VALUE ((0b1 << 2) | (0b1 << 6))
#endif

static MP_DEFINE_CONST_FUN_OBJ_0(Wakeup_get_gpio_state_obj, Wakeup_get_gpio_state);
static MP_DEFINE_CONST_FUN_OBJ_0(Wakeup_reset_gpio_state_obj, Wakeup_reset_gpio_state);
static MP_DEFINE_CONST_FUN_OBJ_0(Wakeup_get_shift_state_obj, Wakeup_get_shift_state);
static MP_DEFINE_CONST_FUN_OBJ_0(Wakeup_reset_shift_state_obj, Wakeup_reset_shift_state);

static const mp_map_elem_t wakeup_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_wakeup) },
    { MP_ROM_QSTR(MP_QSTR_get_gpio_state), MP_ROM_PTR(&Wakeup_get_gpio_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_gpio_state), MP_ROM_PTR(&Wakeup_reset_gpio_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_shift_state), MP_ROM_PTR(&Wakeup_get_shift_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_shift_state), MP_ROM_PTR(&Wakeup_reset_shift_state_obj) }
};
static MP_DEFINE_CONST_DICT(mp_module_wakeup_globals, wakeup_globals_table);

const mp_obj_module_t wakeup_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_wakeup_globals,
};

MP_REGISTER_MODULE(MP_QSTR_wakeup, wakeup_user_cmodule);
