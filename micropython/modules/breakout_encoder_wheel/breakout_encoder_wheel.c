#include "breakout_encoder_wheel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutEncoderWheel Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_address_obj, 2, BreakoutEncoderWheel_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_get_interrupt_flag_obj, BreakoutEncoderWheel_get_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_clear_interrupt_flag_obj, BreakoutEncoderWheel_clear_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_get_direction_obj, BreakoutEncoderWheel_get_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_direction_obj, 2, BreakoutEncoderWheel_set_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_brightness_obj, 2, BreakoutEncoderWheel_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_led_obj, 4, BreakoutEncoderWheel_set_led);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_available_obj, BreakoutEncoderWheel_available);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_read_obj, BreakoutEncoderWheel_read);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_clear_obj, BreakoutEncoderWheel_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutEncoderWheel_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutEncoderWheel_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_interrupt_flag), MP_ROM_PTR(&BreakoutEncoderWheel_get_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupt_flag), MP_ROM_PTR(&BreakoutEncoderWheel_clear_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_direction), MP_ROM_PTR(&BreakoutEncoderWheel_get_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_direction), MP_ROM_PTR(&BreakoutEncoderWheel_set_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&BreakoutEncoderWheel_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&BreakoutEncoderWheel_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_available), MP_ROM_PTR(&BreakoutEncoderWheel_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutEncoderWheel_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutEncoderWheel_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CW), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CCW), MP_ROM_INT(0) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutEncoderWheel_locals_dict, BreakoutEncoderWheel_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_encoder_wheel_BreakoutEncoderWheel_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutEncoderWheel,
    .make_new = BreakoutEncoderWheel_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutEncoderWheel_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_encoder_wheel Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_encoder_wheel_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_encoder_wheel) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutEncoderWheel), (mp_obj_t)&breakout_encoder_wheel_BreakoutEncoderWheel_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_encoder_wheel_globals, breakout_encoder_wheel_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_encoder_wheel_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_encoder_wheel_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder_wheel, breakout_encoder_wheel_user_cmodule, MODULE_BREAKOUT_ENCODER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder_wheel, breakout_encoder_wheel_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////