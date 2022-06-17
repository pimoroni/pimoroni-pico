#include "pico_explorer.h"

const mp_rom_obj_tuple_t PicoExplorer_MOTOR1_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(9), MP_ROM_INT(8) },
};

const mp_rom_obj_tuple_t PicoExplorer_MOTOR2_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(11), MP_ROM_INT(10) },
};

STATIC const mp_map_elem_t picoexplorer_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picoexplorer) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(15) },

    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_INT(26) },
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_ADC2), MP_ROM_INT(28) },

    { MP_ROM_QSTR(MP_QSTR_MOTOR_1), MP_ROM_PTR(&PicoExplorer_MOTOR1_pins) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_2), MP_ROM_PTR(&PicoExplorer_MOTOR2_pins) },

    { MP_ROM_QSTR(MP_QSTR_GP0), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_GP1), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_GP3), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_GP4), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_GP5), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_GP6), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_GP7), MP_ROM_INT(7) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picoexplorer_globals, picoexplorer_globals_table);

const mp_obj_module_t picoexplorer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picoexplorer_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_picoexplorer, picoexplorer_user_cmodule, MODULE_PICOEXPLORER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_picoexplorer, picoexplorer_user_cmodule);
#endif