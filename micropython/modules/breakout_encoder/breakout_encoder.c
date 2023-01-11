#include "breakout_encoder.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutEncoder Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoder_set_address_obj, 2, BreakoutEncoder_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_get_interrupt_flag_obj, BreakoutEncoder_get_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_clear_interrupt_flag_obj, BreakoutEncoder_clear_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_get_direction_obj, BreakoutEncoder_get_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoder_set_direction_obj, 2, BreakoutEncoder_set_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoder_set_brightness_obj, 2, BreakoutEncoder_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoder_set_led_obj, 4, BreakoutEncoder_set_led);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_available_obj, BreakoutEncoder_available);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_read_obj, BreakoutEncoder_read);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoder_clear_obj, BreakoutEncoder_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutEncoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutEncoder_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_interrupt_flag), MP_ROM_PTR(&BreakoutEncoder_get_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupt_flag), MP_ROM_PTR(&BreakoutEncoder_clear_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_direction), MP_ROM_PTR(&BreakoutEncoder_get_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_direction), MP_ROM_PTR(&BreakoutEncoder_set_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&BreakoutEncoder_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&BreakoutEncoder_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_available), MP_ROM_PTR(&BreakoutEncoder_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutEncoder_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutEncoder_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CW), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_CCW), MP_ROM_INT(0) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutEncoder_locals_dict, BreakoutEncoder_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_encoder_BreakoutEncoder_type,
    MP_QSTR_BreakoutEncoder,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutEncoder_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutEncoder_locals_dict
);
#else
const mp_obj_type_t breakout_encoder_BreakoutEncoder_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutEncoder,
    .make_new = BreakoutEncoder_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutEncoder_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_encoder Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_encoder_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_encoder) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutEncoder), (mp_obj_t)&breakout_encoder_BreakoutEncoder_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_encoder_globals, breakout_encoder_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_encoder_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_encoder_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder, breakout_encoder_user_cmodule, MODULE_BREAKOUT_ENCODER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder, breakout_encoder_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////