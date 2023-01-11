#include "encoder.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Encoder___del___obj, Encoder___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_pins_obj, Encoder_pins);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_common_pin_obj, Encoder_common_pin);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_state_obj, Encoder_state);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_count_obj, Encoder_count);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_delta_obj, Encoder_delta);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_zero_obj, Encoder_zero);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_step_obj, Encoder_step);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_turn_obj, Encoder_turn);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_revolutions_obj, Encoder_revolutions);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_degrees_obj, Encoder_degrees);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_radians_obj, Encoder_radians);
MP_DEFINE_CONST_FUN_OBJ_KW(Encoder_direction_obj, 1, Encoder_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(Encoder_counts_per_rev_obj, 1, Encoder_counts_per_rev);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_capture_obj, Encoder_capture);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Encoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Encoder___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&Encoder_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_common_pin), MP_ROM_PTR(&Encoder_common_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&Encoder_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&Encoder_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_delta), MP_ROM_PTR(&Encoder_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_zero), MP_ROM_PTR(&Encoder_zero_obj) },
    { MP_ROM_QSTR(MP_QSTR_step), MP_ROM_PTR(&Encoder_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_turn), MP_ROM_PTR(&Encoder_turn_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions), MP_ROM_PTR(&Encoder_revolutions_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees), MP_ROM_PTR(&Encoder_degrees_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians), MP_ROM_PTR(&Encoder_radians_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&Encoder_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_counts_per_rev), MP_ROM_PTR(&Encoder_counts_per_rev_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&Encoder_capture_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Encoder_locals_dict, Encoder_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    Encoder_type,
    MP_QSTR_encoder,
    MP_TYPE_FLAG_NONE,
    make_new, Encoder_make_new,
    print, Encoder_print,
    locals_dict, (mp_obj_dict_t*)&Encoder_locals_dict
);
#else
const mp_obj_type_t Encoder_type = {
    { &mp_type_type },
    .name = MP_QSTR_encoder,
    .print = Encoder_print,
    .make_new = Encoder_make_new,
    .locals_dict = (mp_obj_dict_t*)&Encoder_locals_dict,
};
#endif

/***** Globals Table *****/
STATIC const mp_map_elem_t encoder_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_encoder) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Encoder), (mp_obj_t)&Encoder_type },

    { MP_ROM_QSTR(MP_QSTR_MMME_CPR), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_ROTARY_CPR), MP_ROM_INT(24) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_encoder_globals, encoder_globals_table);

/***** Module Definition *****/
const mp_obj_module_t encoder_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_encoder_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_encoder, encoder_user_cmodule, MODULE_ENCODER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_encoder, encoder_user_cmodule);
#endif
