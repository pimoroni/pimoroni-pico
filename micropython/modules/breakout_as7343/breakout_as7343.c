#include "breakout_as7343.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutAS7343 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7343_reset_obj, BreakoutAS7343_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7343_version_obj, BreakoutAS7343_version);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7343_read_obj, BreakoutAS7343_read);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_gain_obj, BreakoutAS7343_set_gain);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_integration_time_obj, BreakoutAS7343_set_integration_time);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_measurement_time_obj, BreakoutAS7343_set_measurement_time);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_illumination_current_obj, BreakoutAS7343_set_illumination_current);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_illumination_led_obj, BreakoutAS7343_set_illumination_led);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7343_set_channels_obj, BreakoutAS7343_set_channels);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutAS7343_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&BreakoutAS7343_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&BreakoutAS7343_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutAS7343_read_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_gain), MP_ROM_PTR(&BreakoutAS7343_set_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_measurement_time), MP_ROM_PTR(&BreakoutAS7343_set_measurement_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_integration_time), MP_ROM_PTR(&BreakoutAS7343_set_integration_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_illumination_current), MP_ROM_PTR(&BreakoutAS7343_set_illumination_current_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_illumination_led), MP_ROM_PTR(&BreakoutAS7343_set_illumination_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_channels), MP_ROM_PTR(&BreakoutAS7343_set_channels_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutAS7343_locals_dict, BreakoutAS7343_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_as7343_BreakoutAS7343_type,
    MP_QSTR_BreakoutAS7343,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutAS7343_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutAS7343_locals_dict
);
#else
const mp_obj_type_t breakout_as7343_BreakoutAS7343_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutAS7343,
    .make_new = BreakoutAS7343_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutAS7343_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_as7343 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_as7343_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_as7343) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutAS7343), (mp_obj_t)&breakout_as7343_BreakoutAS7343_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_as7343_globals, breakout_as7343_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_as7343_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_as7343_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_as7343, breakout_as7343_user_cmodule, MODULE_BREAKOUT_AS7343_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_as7343, breakout_as7343_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////