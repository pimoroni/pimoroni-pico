#include "breakout_ltr559.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutLTR559 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutLTR559_part_id_obj, BreakoutLTR559_part_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutLTR559_revision_id_obj, BreakoutLTR559_revision_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutLTR559_manufacturer_id_obj, BreakoutLTR559_manufacturer_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutLTR559_get_reading_obj, BreakoutLTR559_get_reading);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_interrupts_obj, 3, BreakoutLTR559_interrupts);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_proximity_led_obj, 5, BreakoutLTR559_proximity_led);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_light_control_obj, 3, BreakoutLTR559_light_control);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_proximity_control_obj, 3, BreakoutLTR559_proximity_control);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_light_threshold_obj, 3, BreakoutLTR559_light_threshold);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_proximity_threshold_obj, 3, BreakoutLTR559_proximity_threshold);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_light_measurement_rate_obj, 3, BreakoutLTR559_light_measurement_rate);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_proximity_measurement_rate_obj, 2, BreakoutLTR559_proximity_measurement_rate);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutLTR559_proximity_offset_obj, 2, BreakoutLTR559_proximity_offset);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutLTR559_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_part_id), MP_ROM_PTR(&BreakoutLTR559_part_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_revision_id), MP_ROM_PTR(&BreakoutLTR559_revision_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_manufacturer_id), MP_ROM_PTR(&BreakoutLTR559_manufacturer_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_reading), MP_ROM_PTR(&BreakoutLTR559_get_reading_obj) },
    { MP_ROM_QSTR(MP_QSTR_interrupts), MP_ROM_PTR(&BreakoutLTR559_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_proximity_led), MP_ROM_PTR(&BreakoutLTR559_proximity_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_light_control), MP_ROM_PTR(&BreakoutLTR559_light_control_obj) },
    { MP_ROM_QSTR(MP_QSTR_proximity_control), MP_ROM_PTR(&BreakoutLTR559_proximity_control_obj) },
    { MP_ROM_QSTR(MP_QSTR_light_threshold), MP_ROM_PTR(&BreakoutLTR559_light_threshold_obj) },
    { MP_ROM_QSTR(MP_QSTR_proximity_threshold), MP_ROM_PTR(&BreakoutLTR559_proximity_threshold_obj) },
    { MP_ROM_QSTR(MP_QSTR_light_measurement_rate), MP_ROM_PTR(&BreakoutLTR559_light_measurement_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_proximity_measurement_rate), MP_ROM_PTR(&BreakoutLTR559_proximity_measurement_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_proximity_offset), MP_ROM_PTR(&BreakoutLTR559_proximity_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_PROXIMITY), MP_ROM_INT(PROXIMITY) },
    { MP_ROM_QSTR(MP_QSTR_ALS_0), MP_ROM_INT(ALS_0) },
    { MP_ROM_QSTR(MP_QSTR_ALS_1), MP_ROM_INT(ALS_1) },
    { MP_ROM_QSTR(MP_QSTR_INTEGRATION_TIME), MP_ROM_INT(INTEGRATION_TIME) },
    { MP_ROM_QSTR(MP_QSTR_GAIN), MP_ROM_INT(GAIN) },
    { MP_ROM_QSTR(MP_QSTR_RATIO), MP_ROM_INT(RATIO) },
    { MP_ROM_QSTR(MP_QSTR_LUX), MP_ROM_INT(LUX) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutLTR559_locals_dict, BreakoutLTR559_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_ltr559_BreakoutLTR559_type,
    MP_QSTR_BreakoutLTR559,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutLTR559_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutLTR559_locals_dict
);
#else
const mp_obj_type_t breakout_ltr559_BreakoutLTR559_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutLTR559,
    .make_new = BreakoutLTR559_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutLTR559_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_ltr559 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_ltr559_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_ltr559) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutLTR559), (mp_obj_t)&breakout_ltr559_BreakoutLTR559_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_ltr559_globals, breakout_ltr559_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_ltr559_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_ltr559_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_ltr559, breakout_ltr559_user_cmodule, MODULE_BREAKOUT_LTR559_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_ltr559, breakout_ltr559_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////