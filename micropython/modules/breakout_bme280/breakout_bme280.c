#include "breakout_bme280.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutBME280 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBME280_read_obj, BreakoutBME280_read);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutBME280_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutBME280_read_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutBME280_locals_dict, BreakoutBME280_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_bme280_BreakoutBME280_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_bme280,
    .print = BreakoutBME280_print,
    .make_new = BreakoutBME280_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutBME280_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_bme280 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_bme280_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_bme280) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutBME280), (mp_obj_t)&breakout_bme280_BreakoutBME280_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_bme280_globals, breakout_bme280_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_bme280_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_bme280_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_bme280, breakout_bme280_user_cmodule, MODULE_BREAKOUT_BME280_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////