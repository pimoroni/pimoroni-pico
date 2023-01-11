#include "breakout_potentiometer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutPotentiometer Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_address_obj, 2, BreakoutPotentiometer_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_get_adc_vref_obj, BreakoutPotentiometer_get_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_adc_vref_obj, 2, BreakoutPotentiometer_set_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_get_direction_obj, BreakoutPotentiometer_get_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_direction_obj, 2, BreakoutPotentiometer_set_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_brightness_obj, 2, BreakoutPotentiometer_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPotentiometer_set_led_obj, 4, BreakoutPotentiometer_set_led);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_read_obj, BreakoutPotentiometer_read);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPotentiometer_read_raw_obj, BreakoutPotentiometer_read_raw);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutPotentiometer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutPotentiometer_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_adc_vref), MP_ROM_PTR(&BreakoutPotentiometer_get_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_adc_vref), MP_ROM_PTR(&BreakoutPotentiometer_set_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_direction), MP_ROM_PTR(&BreakoutPotentiometer_get_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_direction), MP_ROM_PTR(&BreakoutPotentiometer_set_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&BreakoutPotentiometer_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&BreakoutPotentiometer_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutPotentiometer_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_raw), MP_ROM_PTR(&BreakoutPotentiometer_read_raw_obj) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CW), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CCW), MP_ROM_INT(0) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutPotentiometer_locals_dict, BreakoutPotentiometer_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_potentiometer_BreakoutPotentiometer_type,
    MP_QSTR_BreakoutPotentiometer,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutPotentiometer_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutPotentiometer_locals_dict
);
#else
const mp_obj_type_t breakout_potentiometer_BreakoutPotentiometer_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutPotentiometer,
    .make_new = BreakoutPotentiometer_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutPotentiometer_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_potentiometer Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_potentiometer_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_potentiometer) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutPotentiometer), (mp_obj_t)&breakout_potentiometer_BreakoutPotentiometer_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_potentiometer_globals, breakout_potentiometer_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_potentiometer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_potentiometer_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_potentiometer, breakout_potentiometer_user_cmodule, MODULE_BREAKOUT_POTENTIOMETER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_potentiometer, breakout_potentiometer_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////