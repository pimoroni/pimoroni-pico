#include "breakout_msa301.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutMSA301 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_part_id_obj, BreakoutMSA301_part_id);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_axis_obj, 2, BreakoutMSA301_get_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_x_axis_obj, 1, BreakoutMSA301_get_x_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_y_axis_obj, 1, BreakoutMSA301_get_y_axis);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_get_z_axis_obj, 1, BreakoutMSA301_get_z_axis);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_get_orientation_obj, BreakoutMSA301_get_orientation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_power_mode_obj, 2, BreakoutMSA301_set_power_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_range_and_resolution_obj, 2, BreakoutMSA301_set_range_and_resolution);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_axis_polarity_obj, 2, BreakoutMSA301_set_axis_polarity);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMSA301_disable_all_interrupts_obj, BreakoutMSA301_disable_all_interrupts);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_enable_interrupts_obj, 2, BreakoutMSA301_enable_interrupts);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_set_interrupt_latch_obj, 3, BreakoutMSA301_set_interrupt_latch);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMSA301_read_interrupt_obj, 2, BreakoutMSA301_read_interrupt);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutMSA301_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_part_id), MP_ROM_PTR(&BreakoutMSA301_part_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_axis), MP_ROM_PTR(&BreakoutMSA301_get_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_x_axis), MP_ROM_PTR(&BreakoutMSA301_get_x_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_y_axis), MP_ROM_PTR(&BreakoutMSA301_get_y_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_z_axis), MP_ROM_PTR(&BreakoutMSA301_get_z_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_orientation), MP_ROM_PTR(&BreakoutMSA301_get_orientation_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_power_mode), MP_ROM_PTR(&BreakoutMSA301_set_power_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_range_and_resolution), MP_ROM_PTR(&BreakoutMSA301_set_range_and_resolution_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_axis_polarity), MP_ROM_PTR(&BreakoutMSA301_set_axis_polarity_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_all_interrupts), MP_ROM_PTR(&BreakoutMSA301_disable_all_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupts), MP_ROM_PTR(&BreakoutMSA301_enable_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_interrupt_latch), MP_ROM_PTR(&BreakoutMSA301_set_interrupt_latch_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_interrupt), MP_ROM_PTR(&BreakoutMSA301_read_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_X), MP_ROM_INT(MSA_AXIS_X) },
    { MP_ROM_QSTR(MP_QSTR_Y), MP_ROM_INT(MSA_AXIS_Y) },
    { MP_ROM_QSTR(MP_QSTR_Z), MP_ROM_INT(MSA_AXIS_Z) },
    { MP_ROM_QSTR(MP_QSTR_PORTRAIT), MP_ROM_INT(MSA_PORTRAIT) },
    { MP_ROM_QSTR(MP_QSTR_PORTRAIT_INVERTED), MP_ROM_INT(MSA_PORTRAIT_INVERTED) },
    { MP_ROM_QSTR(MP_QSTR_LANDSCAPE), MP_ROM_INT(MSA_LANDSCAPE) },
    { MP_ROM_QSTR(MP_QSTR_LANDSCAPE_INVERTED), MP_ROM_INT(MSA_LANDSCAPE_INVERTED) },
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(MSA_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_LOW), MP_ROM_INT(MSA_LOW) },
    { MP_ROM_QSTR(MP_QSTR_SUSPEND), MP_ROM_INT(MSA_SUSPEND) },
    { MP_ROM_QSTR(MP_QSTR_G_2), MP_ROM_INT(MSA_G_2) },
    { MP_ROM_QSTR(MP_QSTR_G_4), MP_ROM_INT(MSA_G_4) },
    { MP_ROM_QSTR(MP_QSTR_G_8), MP_ROM_INT(MSA_G_8) },
    { MP_ROM_QSTR(MP_QSTR_G_16), MP_ROM_INT(MSA_G_16) },
    { MP_ROM_QSTR(MP_QSTR_BITS_14), MP_ROM_INT(MSA_BITS_14) },
    { MP_ROM_QSTR(MP_QSTR_BITS_12), MP_ROM_INT(MSA_BITS_12) },
    { MP_ROM_QSTR(MP_QSTR_BITS_10), MP_ROM_INT(MSA_BITS_10) },
    { MP_ROM_QSTR(MP_QSTR_BITS_8), MP_ROM_INT(MSA_BITS_8) },
    { MP_ROM_QSTR(MP_QSTR_INVERT_X), MP_ROM_INT(MSA_INVERT_X) },
    { MP_ROM_QSTR(MP_QSTR_INVERT_Y), MP_ROM_INT(MSA_INVERT_Y) },
    { MP_ROM_QSTR(MP_QSTR_INVERT_Z), MP_ROM_INT(MSA_INVERT_Z) },
    { MP_ROM_QSTR(MP_QSTR_XY_SWAP), MP_ROM_INT(MSA_XY_SWAP) },
    { MP_ROM_QSTR(MP_QSTR_NONE), MP_ROM_INT(MSA_NONE) },
    { MP_ROM_QSTR(MP_QSTR_ACTIVE), MP_ROM_INT(MSA_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_NEW_DATA), MP_ROM_INT(MSA_NEW_DATA) },
    { MP_ROM_QSTR(MP_QSTR_FREEFALL), MP_ROM_INT(MSA_FREEFALL) },
    { MP_ROM_QSTR(MP_QSTR_ORIENTATION), MP_ROM_INT(MSA_ORIENTATION) },
    { MP_ROM_QSTR(MP_QSTR_SINGLE_TAP), MP_ROM_INT(MSA_SINGLE_TAP) },
    { MP_ROM_QSTR(MP_QSTR_DOUBLE_TAP), MP_ROM_INT(MSA_DOUBLE_TAP) },
    { MP_ROM_QSTR(MP_QSTR_Z_ACTIVE), MP_ROM_INT(MSA_Z_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_Y_ACTIVE), MP_ROM_INT(MSA_Y_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_X_ACTIVE), MP_ROM_INT(MSA_X_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_1MS), MP_ROM_INT(MSA_LATCH_1MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_2MS), MP_ROM_INT(MSA_LATCH_2MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_25MS), MP_ROM_INT(MSA_LATCH_25MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_50MS), MP_ROM_INT(MSA_LATCH_50MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_100MS), MP_ROM_INT(MSA_LATCH_100MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_250MS), MP_ROM_INT(MSA_LATCH_250MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_500MS), MP_ROM_INT(MSA_LATCH_500MS) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_1S), MP_ROM_INT(MSA_LATCH_1S) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_2S), MP_ROM_INT(MSA_LATCH_2S) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_4S), MP_ROM_INT(MSA_LATCH_4S) },
    { MP_ROM_QSTR(MP_QSTR_LATCH_8S), MP_ROM_INT(MSA_LATCH_8S) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutMSA301_locals_dict, BreakoutMSA301_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_msa301_BreakoutMSA301_type,
    MP_QSTR_BreakoutMSA301,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutMSA301_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutMSA301_locals_dict
);
#else
const mp_obj_type_t breakout_msa301_BreakoutMSA301_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutMSA301,
    .make_new = BreakoutMSA301_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutMSA301_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_msa301 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_msa301_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_msa301) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutMSA301), (mp_obj_t)&breakout_msa301_BreakoutMSA301_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_msa301_globals, breakout_msa301_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_msa301_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_msa301_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_msa301, breakout_msa301_user_cmodule, MODULE_BREAKOUT_MSA301_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_msa301, breakout_msa301_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////