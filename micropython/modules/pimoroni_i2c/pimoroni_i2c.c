#include "pimoroni_i2c.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PimoroniI2C Class
////////////////////////////////////////////////////////////////////////////////////////////////////


/***** Methods *****/
//MP_DEFINE_CONST_FUN_OBJ_KW(PimoroniI2C_read_obj, 2, PimoroniI2C_set_address);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t PimoroniI2C_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(PimoroniI2C_locals_dict, PimoroniI2C_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t PimoroniI2C_type = {
    { &mp_type_type },
    .name = MP_QSTR_pimoroni_i2c,
    .print = PimoroniI2C_print,
    .make_new = PimoroniI2C_make_new,
    .locals_dict = (mp_obj_dict_t*)&PimoroniI2C_locals_dict,
};


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
MP_REGISTER_MODULE(MP_QSTR_pimoroni_i2c, pimoroni_i2c_user_cmodule, MODULE_PIMORONI_I2C_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////