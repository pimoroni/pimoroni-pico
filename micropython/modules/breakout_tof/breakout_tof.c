#include "breakout_tof.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutTOF Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_id_obj, BreakoutTOF_get_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_osc_obj, BreakoutTOF_get_osc);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_osc_obj, 2, BreakoutTOF_set_osc);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_address_obj, 2, BreakoutTOF_set_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_address_obj, BreakoutTOF_get_address);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_distance_mode_obj, 2, BreakoutTOF_set_distance_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_distance_mode_int_obj, 2, BreakoutTOF_set_distance_mode_int);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_distance_mode_obj, BreakoutTOF_get_distance_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_measurement_timing_budget_obj, 2, BreakoutTOF_set_measurement_timing_budget);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_measurement_timing_budget_obj, BreakoutTOF_get_measurement_timing_budget);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_start_continuous_obj, 2, BreakoutTOF_start_continuous);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_stop_continuous_obj, BreakoutTOF_stop_continuous);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_read_obj, 1, BreakoutTOF_read);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_read_range_continuous_millimeters_obj, 1, BreakoutTOF_read_range_continuous_millimeters);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_read_single_obj, 1, BreakoutTOF_read_single);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_read_range_single_millimeters_obj, 1, BreakoutTOF_read_range_single_millimeters);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_data_ready_obj, BreakoutTOF_data_ready);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_range_status_to_string_obj, 2, BreakoutTOF_range_status_to_string);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTOF_set_timeout_obj, 2, BreakoutTOF_set_timeout);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_timeout_obj, BreakoutTOF_get_timeout);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_timeout_occurred_obj, BreakoutTOF_timeout_occurred);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTOF_get_curr_ms_obj, BreakoutTOF_get_curr_ms);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutTOF_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_id), MP_ROM_PTR(&BreakoutTOF_get_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_osc), MP_ROM_PTR(&BreakoutTOF_get_osc_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_osc), MP_ROM_PTR(&BreakoutTOF_set_osc_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_address), MP_ROM_PTR(&BreakoutTOF_set_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_address), MP_ROM_PTR(&BreakoutTOF_get_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_distance_mode), MP_ROM_PTR(&BreakoutTOF_set_distance_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_distance_mode_int), MP_ROM_PTR(&BreakoutTOF_set_distance_mode_int_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_distance_mode), MP_ROM_PTR(&BreakoutTOF_get_distance_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_measurement_timing_budget), MP_ROM_PTR(&BreakoutTOF_set_measurement_timing_budget_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_measurement_timing_budget), MP_ROM_PTR(&BreakoutTOF_get_measurement_timing_budget_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_continuous), MP_ROM_PTR(&BreakoutTOF_start_continuous_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_continuous), MP_ROM_PTR(&BreakoutTOF_stop_continuous_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutTOF_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_range_continuous_millimeters), MP_ROM_PTR(&BreakoutTOF_read_range_continuous_millimeters_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_single), MP_ROM_PTR(&BreakoutTOF_read_single_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_range_single_millimeters), MP_ROM_PTR(&BreakoutTOF_read_range_single_millimeters_obj) },
    { MP_ROM_QSTR(MP_QSTR_data_ready), MP_ROM_PTR(&BreakoutTOF_data_ready_obj) },
    { MP_ROM_QSTR(MP_QSTR_range_status_to_string), MP_ROM_PTR(&BreakoutTOF_range_status_to_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_timeout), MP_ROM_PTR(&BreakoutTOF_set_timeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_timeout), MP_ROM_PTR(&BreakoutTOF_get_timeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout_occurred), MP_ROM_PTR(&BreakoutTOF_timeout_occurred_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_curr_ms), MP_ROM_PTR(&BreakoutTOF_get_curr_ms_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutTOF_locals_dict, BreakoutTOF_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_tof_BreakoutTOF_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_tof,
    .print = BreakoutTOF_print,
    .make_new = BreakoutTOF_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutTOF_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_tof Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_tof_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_tof) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutTOF), (mp_obj_t)&breakout_tof_BreakoutTOF_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_tof_globals, breakout_tof_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_tof_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_tof_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_tof, breakout_tof_user_cmodule, MODULE_BREAKOUT_TOF_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////