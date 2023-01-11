#include "breakout_bh1745.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutBH1745 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_chip_id_obj, BreakoutBH1745_chip_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_manufacturer_id_obj, BreakoutBH1745_manufacturer_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_raw_obj, BreakoutBH1745_rgbc_raw);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_clamped_obj, BreakoutBH1745_rgbc_clamped);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBH1745_rgbc_scaled_obj, BreakoutBH1745_rgbc_scaled);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBH1745_leds_obj, 2, BreakoutBH1745_leds);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBH1745_threshold_obj, 3, BreakoutBH1745_threshold);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBH1745_measurement_time_ms_obj, 2, BreakoutBH1745_measurement_time_ms);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutBH1745_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_chip_id), MP_ROM_PTR(&BreakoutBH1745_chip_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_manufacturer_id), MP_ROM_PTR(&BreakoutBH1745_manufacturer_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgbc_raw), MP_ROM_PTR(&BreakoutBH1745_rgbc_raw_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgbc_clamped), MP_ROM_PTR(&BreakoutBH1745_rgbc_clamped_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgbc_scaled), MP_ROM_PTR(&BreakoutBH1745_rgbc_scaled_obj) },
    { MP_ROM_QSTR(MP_QSTR_threshold), MP_ROM_PTR(&BreakoutBH1745_threshold_obj) },
    { MP_ROM_QSTR(MP_QSTR_measurement_time_ms), MP_ROM_PTR(&BreakoutBH1745_measurement_time_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_leds), MP_ROM_PTR(&BreakoutBH1745_leds_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutBH1745_locals_dict, BreakoutBH1745_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_bh1745_BreakoutBH1745_type,
    MP_QSTR_BreakoutBH1745,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutBH1745_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutBH1745_locals_dict
);
#else
const mp_obj_type_t breakout_bh1745_BreakoutBH1745_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutBH1745,
    .make_new = BreakoutBH1745_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutBH1745_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_bh1745 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_bh1745_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_bh1745) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutBH1745), (mp_obj_t)&breakout_bh1745_BreakoutBH1745_type },

    { MP_ROM_QSTR(MP_QSTR_I2C_ADDRESS_DEFAULT), MP_ROM_INT(BH1745_I2C_ADDRESS_DEFAULT) },
    { MP_ROM_QSTR(MP_QSTR_I2C_ADDRESS_ALT), MP_ROM_INT(BH1745_I2C_ADDRESS_ALTERNATE) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_bh1745_globals, breakout_bh1745_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_bh1745_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_bh1745_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_bh1745, breakout_bh1745_user_cmodule, MODULE_BREAKOUT_LTR559_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_bh1745, breakout_bh1745_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////