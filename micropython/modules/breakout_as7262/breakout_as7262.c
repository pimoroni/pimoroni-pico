#include "breakout_as7262.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutAS7262 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_reset_obj, BreakoutAS7262_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_device_type_obj, BreakoutAS7262_device_type);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_hardware_version_obj, BreakoutAS7262_hardware_version);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_firmware_version_obj, BreakoutAS7262_firmware_version);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_read_obj, BreakoutAS7262_read);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutAS7262_temperature_obj, BreakoutAS7262_temperature);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7262_set_gain_obj, BreakoutAS7262_set_gain);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7262_set_measurement_mode_obj, BreakoutAS7262_set_measurement_mode);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7262_set_indicator_current_obj, BreakoutAS7262_set_indicator_current);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7262_set_illumination_current_obj, BreakoutAS7262_set_illumination_current);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutAS7262_set_integration_time_obj, BreakoutAS7262_set_integration_time);
MP_DEFINE_CONST_FUN_OBJ_3(BreakoutAS7262_set_leds_obj, BreakoutAS7262_set_leds);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutAS7262_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&BreakoutAS7262_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_device_type), MP_ROM_PTR(&BreakoutAS7262_device_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_hardware_version), MP_ROM_PTR(&BreakoutAS7262_hardware_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_firmware_version), MP_ROM_PTR(&BreakoutAS7262_firmware_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutAS7262_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_temperature), MP_ROM_PTR(&BreakoutAS7262_temperature_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_gain), MP_ROM_PTR(&BreakoutAS7262_set_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_measurement_mode), MP_ROM_PTR(&BreakoutAS7262_set_measurement_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_indicator_current), MP_ROM_PTR(&BreakoutAS7262_set_indicator_current_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_illumination_current), MP_ROM_PTR(&BreakoutAS7262_set_illumination_current_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_integration_time), MP_ROM_PTR(&BreakoutAS7262_set_integration_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_leds), MP_ROM_PTR(&BreakoutAS7262_set_leds_obj) },

    { MP_ROM_QSTR(MP_QSTR_X1), MP_ROM_INT(MP_X1) },
    { MP_ROM_QSTR(MP_QSTR_X3_7), MP_ROM_INT(MP_X3_7) },
    { MP_ROM_QSTR(MP_QSTR_X16), MP_ROM_INT(MP_X16) },
    { MP_ROM_QSTR(MP_QSTR_X64), MP_ROM_INT(MP_X64) },

    { MP_ROM_QSTR(MP_QSTR_MA12), MP_ROM_INT(MP_MA12) },
    { MP_ROM_QSTR(MP_QSTR_MA25), MP_ROM_INT(MP_MA25) },
    { MP_ROM_QSTR(MP_QSTR_MA50), MP_ROM_INT(MP_MA50) },
    { MP_ROM_QSTR(MP_QSTR_MA100), MP_ROM_INT(MP_MA100) },

    { MP_ROM_QSTR(MP_QSTR_MA1), MP_ROM_INT(MP_MA1) },
    { MP_ROM_QSTR(MP_QSTR_MA2), MP_ROM_INT(MP_MA2) },
    { MP_ROM_QSTR(MP_QSTR_MA4), MP_ROM_INT(MP_MA4) },
    { MP_ROM_QSTR(MP_QSTR_MA8), MP_ROM_INT(MP_MA8) },

    { MP_ROM_QSTR(MP_QSTR_CONT_YGNV), MP_ROM_INT(MP_CONT_YGNV) },
    { MP_ROM_QSTR(MP_QSTR_CONT_ROYG), MP_ROM_INT(MP_CONT_ROYG) },
    { MP_ROM_QSTR(MP_QSTR_CONT_ROYGBR), MP_ROM_INT(MP_CONT_ROYGBR) },
    { MP_ROM_QSTR(MP_QSTR_ONESHOT), MP_ROM_INT(MP_ONESHOT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutAS7262_locals_dict, BreakoutAS7262_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_as7262_BreakoutAS7262_type,
    MP_QSTR_BreakoutAS7262,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutAS7262_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutAS7262_locals_dict
);
#else
const mp_obj_type_t breakout_as7262_BreakoutAS7262_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutAS7262,
    .make_new = BreakoutAS7262_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutAS7262_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_as7262 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_as7262_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_as7262) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutAS7262), (mp_obj_t)&breakout_as7262_BreakoutAS7262_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_as7262_globals, breakout_as7262_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_as7262_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_as7262_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_as7262, breakout_as7262_user_cmodule, MODULE_BREAKOUT_AS7262_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_as7262, breakout_as7262_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////