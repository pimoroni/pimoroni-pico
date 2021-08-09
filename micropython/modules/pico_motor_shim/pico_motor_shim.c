#include "pico_motor_shim.h"

/***** Constants *****/
enum pins
{
    BUTTON_A = 2,

    MOTOR_1_POS = 6,
    MOTOR_1_NEG = 7,

    MOTOR_2_POS = 27,
    MOTOR_2_NEG = 26,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picomotorshim Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t picomotorshim_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picomotorshim) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOTOR_1_POS), MP_ROM_INT(MOTOR_1_POS) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOTOR_1_NEG), MP_ROM_INT(MOTOR_1_NEG) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOTOR_2_POS), MP_ROM_INT(MOTOR_2_POS) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOTOR_2_NEG), MP_ROM_INT(MOTOR_2_NEG) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picomotorshim_globals, picomotorshim_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picomotorshim_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picomotorshim_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picomotorshim, picomotorshim_user_cmodule, MODULE_PICOMOTORSHIM_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////