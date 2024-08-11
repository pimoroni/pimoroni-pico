#include "pimoroni_i2c.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PimoroniI2C Class
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
MP_DEFINE_CONST_FUN_OBJ_1(PimoroniI2C___del___obj, PimoroniI2C___del__);

STATIC const mp_rom_map_elem_t PimoroniI2C_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PimoroniI2C___del___obj) },
};

STATIC MP_DEFINE_CONST_DICT(PimoroniI2C_locals_dict, PimoroniI2C_locals_dict_table);
*/

STATIC const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    PimoroniI2C_type,
    MP_QSTR_pimoroni_i2c,
    MP_TYPE_FLAG_NONE,
    make_new, PimoroniI2C_make_new,
    print, PimoroniI2C_print,
    protocol, &machine_i2c_p,
    locals_dict, (mp_obj_dict_t*)&mp_machine_i2c_locals_dict
);
#else
const mp_obj_type_t PimoroniI2C_type = {
    { &mp_type_type },
    .name = MP_QSTR_pimoroni_i2c,
    .print = PimoroniI2C_print,
    .make_new = PimoroniI2C_make_new,
    .protocol = &machine_i2c_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_i2c_locals_dict,
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_potentiometer Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t pimoroni_i2c_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pimoroni_i2c) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PimoroniI2C), (mp_obj_t)&PimoroniI2C_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_pimoroni_i2c_globals, pimoroni_i2c_globals_table);

/***** Module Definition *****/
const mp_obj_module_t pimoroni_i2c_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_pimoroni_i2c_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_pimoroni_i2c, pimoroni_i2c_user_cmodule, MODULE_PIMORONI_I2C_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_pimoroni_i2c, pimoroni_i2c_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////