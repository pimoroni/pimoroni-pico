#include "encoder.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot___del___obj, Snapshot___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_count_obj, Snapshot_count);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_delta_obj, Snapshot_delta);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_frequency_obj, Snapshot_frequency);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_revolutions_obj, Snapshot_revolutions);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_degrees_obj, Snapshot_degrees);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_radians_obj, Snapshot_radians);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_revolutions_delta_obj, Snapshot_revolutions_delta);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_degrees_delta_obj, Snapshot_degrees_delta);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_radians_delta_obj, Snapshot_radians_delta);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_revolutions_per_second_obj, Snapshot_revolutions_per_second);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_revolutions_per_minute_obj, Snapshot_revolutions_per_minute);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_degrees_per_second_obj, Snapshot_degrees_per_second);
MP_DEFINE_CONST_FUN_OBJ_1(Snapshot_radians_per_second_obj, Snapshot_radians_per_second);

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
MP_DEFINE_CONST_FUN_OBJ_KW(Encoder_counts_per_revolution_obj, 1, Encoder_counts_per_revolution);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_take_snapshot_obj, Encoder_take_snapshot);
MP_DEFINE_CONST_FUN_OBJ_1(Encoder_capture_obj, Encoder_capture);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Snapshot_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Snapshot___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&Snapshot_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_delta), MP_ROM_PTR(&Snapshot_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Snapshot_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions), MP_ROM_PTR(&Snapshot_revolutions_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees), MP_ROM_PTR(&Snapshot_degrees_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians), MP_ROM_PTR(&Snapshot_radians_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions_delta), MP_ROM_PTR(&Snapshot_revolutions_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees_delta), MP_ROM_PTR(&Snapshot_degrees_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians_delta), MP_ROM_PTR(&Snapshot_radians_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions_per_second), MP_ROM_PTR(&Snapshot_revolutions_per_second_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions_per_minute), MP_ROM_PTR(&Snapshot_revolutions_per_minute_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees_per_second), MP_ROM_PTR(&Snapshot_degrees_per_second_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians_per_second), MP_ROM_PTR(&Snapshot_radians_per_second_obj) },
};

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
    { MP_ROM_QSTR(MP_QSTR_counts_per_revolution), MP_ROM_PTR(&Encoder_counts_per_revolution_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_snapshot), MP_ROM_PTR(&Encoder_take_snapshot_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&Encoder_capture_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Snapshot_locals_dict, Snapshot_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(Encoder_locals_dict, Encoder_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t Snapshot_type = {
    { &mp_type_type },
    .name = MP_QSTR_snapshot,
    .print = Snapshot_print,
    .make_new = Snapshot_make_new,
    .locals_dict = (mp_obj_dict_t*)&Snapshot_locals_dict,
};

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
    { MP_OBJ_NEW_QSTR(MP_QSTR_Snapshot), (mp_obj_t)&Snapshot_type },

    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_REVERSED), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_MMME_CPR), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_ROTARY_CPR), MP_ROM_INT(24) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_encoder_globals, encoder_globals_table);

/***** Module Definition *****/
const mp_obj_module_t encoder_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_encoder_globals,
};
MP_REGISTER_MODULE(MP_QSTR_encoder, encoder_user_cmodule, MODULE_ENCODER_ENABLED);
