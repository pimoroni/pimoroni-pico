#include "encoder.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Encoder___del___obj, Encoder___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_pins_obj, Encoder_pins);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_state_obj, Encoder_state);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_count_obj, Encoder_count);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_revolutions_obj, Encoder_revolutions);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_angle_degrees_obj, Encoder_angle_degrees);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_angle_radians_obj, Encoder_angle_radians);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_frequency_obj, Encoder_frequency);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_revolutions_per_second_obj, Encoder_revolutions_per_second);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_revolutions_per_minute_obj, Encoder_revolutions_per_minute);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_degrees_per_second_obj, Encoder_degrees_per_second);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_radians_per_second_obj, Encoder_radians_per_second);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_zero_count_obj, Encoder_zero_count);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_take_snapshot_obj, Encoder_take_snapshot);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Encoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Encoder___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&Encoder_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&Encoder_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&Encoder_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions), MP_ROM_PTR(&Encoder_revolutions_obj) },
    { MP_ROM_QSTR(MP_QSTR_angle_degrees), MP_ROM_PTR(&Encoder_angle_degrees_obj) },
    { MP_ROM_QSTR(MP_QSTR_angle_radians), MP_ROM_PTR(&Encoder_angle_radians_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Encoder_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions_per_second), MP_ROM_PTR(&Encoder_revolutions_per_second_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions_per_minute), MP_ROM_PTR(&Encoder_revolutions_per_minute_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees_per_second), MP_ROM_PTR(&Encoder_degrees_per_second_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians_per_second), MP_ROM_PTR(&Encoder_radians_per_second_obj) },
    { MP_ROM_QSTR(MP_QSTR_zero_count), MP_ROM_PTR(&Encoder_zero_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_snapshot), MP_ROM_PTR(&Encoder_take_snapshot_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Encoder_locals_dict, Encoder_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t Encoder_type = {
    { &mp_type_type },
    .name = MP_QSTR_encoder,
    .print = Encoder_print,
    .make_new = Encoder_make_new,
    .locals_dict = (mp_obj_dict_t*)&Encoder_locals_dict,
};

/***** Globals Table *****/
STATIC const mp_map_elem_t encoder_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_encoder) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Encoder), (mp_obj_t)&Encoder_type },

    { MP_ROM_QSTR(MP_QSTR_ANGULAR), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_LINEAR), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_CONTINUOUS), MP_ROM_INT(0x02) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_encoder_globals, encoder_globals_table);

/***** Module Definition *****/
const mp_obj_module_t encoder_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_encoder_globals,
};
MP_REGISTER_MODULE(MP_QSTR_encoder, encoder_user_cmodule, MODULE_ENCODER_ENABLED);
