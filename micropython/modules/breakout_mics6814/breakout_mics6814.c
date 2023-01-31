#include "breakout_mics6814.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutMICS6814 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMICS6814_set_address_obj, 2, BreakoutMICS6814_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_get_adc_vref_obj, BreakoutMICS6814_get_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMICS6814_set_adc_vref_obj, 2, BreakoutMICS6814_set_adc_vref);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMICS6814_set_brightness_obj, 2, BreakoutMICS6814_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMICS6814_set_led_obj, 4, BreakoutMICS6814_set_led);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMICS6814_set_heater_obj, 2, BreakoutMICS6814_set_heater);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_disable_heater_obj, BreakoutMICS6814_disable_heater);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_get_raw_ref_obj, BreakoutMICS6814_get_raw_ref);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_get_raw_red_obj, BreakoutMICS6814_get_raw_red);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_get_raw_nh3_obj, BreakoutMICS6814_get_raw_nh3);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_get_raw_oxd_obj, BreakoutMICS6814_get_raw_oxd);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_read_all_obj, BreakoutMICS6814_read_all);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_read_ref_obj, BreakoutMICS6814_read_ref);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_read_reducing_obj, BreakoutMICS6814_read_reducing);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_read_nh3_obj, BreakoutMICS6814_read_nh3);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMICS6814_read_oxidising_obj, BreakoutMICS6814_read_oxidising);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutMICS6814_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutMICS6814_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_adc_vref), MP_ROM_PTR(&BreakoutMICS6814_get_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_adc_vref), MP_ROM_PTR(&BreakoutMICS6814_set_adc_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&BreakoutMICS6814_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&BreakoutMICS6814_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_heater), MP_ROM_PTR(&BreakoutMICS6814_set_heater_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_heater), MP_ROM_PTR(&BreakoutMICS6814_disable_heater_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_ref), MP_ROM_PTR(&BreakoutMICS6814_get_raw_ref_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_red), MP_ROM_PTR(&BreakoutMICS6814_get_raw_red_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_nh3), MP_ROM_PTR(&BreakoutMICS6814_get_raw_nh3_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_oxd), MP_ROM_PTR(&BreakoutMICS6814_get_raw_oxd_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_all), MP_ROM_PTR(&BreakoutMICS6814_read_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_ref), MP_ROM_PTR(&BreakoutMICS6814_read_ref_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_reducing), MP_ROM_PTR(&BreakoutMICS6814_read_reducing_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_nh3), MP_ROM_PTR(&BreakoutMICS6814_read_nh3_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_oxidising), MP_ROM_PTR(&BreakoutMICS6814_read_oxidising_obj) },
    { MP_ROM_QSTR(MP_QSTR_REF), MP_ROM_INT(REF) },
    { MP_ROM_QSTR(MP_QSTR_REDUCING), MP_ROM_INT(REDUCING) },
    { MP_ROM_QSTR(MP_QSTR_NH3), MP_ROM_INT(NH3) },
    { MP_ROM_QSTR(MP_QSTR_OXIDISING), MP_ROM_INT(OXIDISING) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutMICS6814_locals_dict, BreakoutMICS6814_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_mics6814_BreakoutMICS6814_type,
    MP_QSTR_BreakoutMICS6814,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutMICS6814_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutMICS6814_locals_dict
);
#else
const mp_obj_type_t breakout_mics6814_BreakoutMICS6814_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutMICS6814,
    .make_new = BreakoutMICS6814_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutMICS6814_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_mics6814 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_mics6814_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_mics6814) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutMICS6814), (mp_obj_t)&breakout_mics6814_BreakoutMICS6814_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_mics6814_globals, breakout_mics6814_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_mics6814_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_mics6814_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_mics6814, breakout_mics6814_user_cmodule, MODULE_BREAKOUT_MICS6814_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_mics6814, breakout_mics6814_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////