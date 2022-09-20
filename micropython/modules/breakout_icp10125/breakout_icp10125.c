#include "breakout_icp10125.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutICP10125 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutICP10125_measure_obj, 1, BreakoutICP10125_measure);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutICP10125_soft_reset_obj, BreakoutICP10125_soft_reset);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutICP10125_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_measure), MP_ROM_PTR(&BreakoutICP10125_measure_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&BreakoutICP10125_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_LOW_POWER), MP_ROM_INT(LOW_POWER) },
    { MP_ROM_QSTR(MP_QSTR_LOW_NOISE), MP_ROM_INT(LOW_NOISE) },
    { MP_ROM_QSTR(MP_QSTR_ULTRA_LOW_NOISE), MP_ROM_INT(ULTRA_LOW_NOISE) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_OK), MP_ROM_INT(OK) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_CRC_FAIL), MP_ROM_INT(CRC_FAIL) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutICP10125_locals_dict, BreakoutICP10125_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_icp10125_BreakoutICP10125_type,
    MP_QSTR_BreakoutICP10125,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutICP10125_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutICP10125_locals_dict
);
#else
const mp_obj_type_t breakout_icp10125_BreakoutICP10125_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutICP10125,
    .make_new = BreakoutICP10125_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutICP10125_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_icp10125 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_icp10125_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_breakout_icp10125) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutICP10125), (mp_obj_t)&breakout_icp10125_BreakoutICP10125_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_icp10125_globals, breakout_icp10125_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_icp10125_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_icp10125_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_icp10125, breakout_icp10125_user_cmodule, MODULE_BREAKOUT_SGP30_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_icp10125, breakout_icp10125_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////