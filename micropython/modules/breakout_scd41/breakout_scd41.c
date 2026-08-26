#include "breakout_scd41.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutSCD41 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_start_periodic_measurement_obj, BreakoutSCD41_start_periodic_measurement);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_start_low_power_periodic_measurement_obj, BreakoutSCD41_start_low_power_periodic_measurement);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_stop_periodic_measurement_obj, BreakoutSCD41_stop_periodic_measurement);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_read_measurement_obj, BreakoutSCD41_read_measurement);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_get_data_ready_obj, BreakoutSCD41_get_data_ready);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutSCD41_set_temperature_offset_obj, BreakoutSCD41_set_temperature_offset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_get_temperature_offset_obj, BreakoutSCD41_get_temperature_offset);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutSCD41_set_sensor_altitude_obj, BreakoutSCD41_set_sensor_altitude);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutSCD41_set_ambient_pressure_obj, BreakoutSCD41_set_ambient_pressure);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutSCD41_perform_forced_recalibration_obj, BreakoutSCD41_perform_forced_recalibration);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutSCD41_set_automatic_self_calibration_obj, BreakoutSCD41_set_automatic_self_calibration);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSCD41_get_automatic_self_calibration_obj, BreakoutSCD41_get_automatic_self_calibration);

/***** Binding of Methods *****/
static const mp_rom_map_elem_t BreakoutSCD41_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&BreakoutSCD41_start_periodic_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_low_power), MP_ROM_PTR(&BreakoutSCD41_start_low_power_periodic_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&BreakoutSCD41_stop_periodic_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure), MP_ROM_PTR(&BreakoutSCD41_read_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_ready), MP_ROM_PTR(&BreakoutSCD41_get_data_ready_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_temperature_offset), MP_ROM_PTR(&BreakoutSCD41_set_temperature_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_temperature_offset), MP_ROM_PTR(&BreakoutSCD41_get_temperature_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_sensor_altitude), MP_ROM_PTR(&BreakoutSCD41_set_sensor_altitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ambient_pressure), MP_ROM_PTR(&BreakoutSCD41_set_ambient_pressure_obj) },

    { MP_ROM_QSTR(MP_QSTR_perform_forced_recalibration), MP_ROM_PTR(&BreakoutSCD41_perform_forced_recalibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_automatic_self_calibration), MP_ROM_PTR(&BreakoutSCD41_set_automatic_self_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_automatic_self_calibration), MP_ROM_PTR(&BreakoutSCD41_get_automatic_self_calibration_obj) },
};
static MP_DEFINE_CONST_DICT(BreakoutSCD41_locals_dict, BreakoutSCD41_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_scd41_BreakoutSCD41_type,
    MP_QSTR_BreakoutSCD41,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutSCD41_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutSCD41_locals_dict
);
#else
const mp_obj_type_t breakout_scd41_BreakoutSCD41_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutSCD41,
    .make_new = BreakoutSCD41_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutSCD41_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_scd41 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
static const mp_map_elem_t scd41_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_breakout_scd41) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutSCD41), (mp_obj_t)&breakout_scd41_BreakoutSCD41_type },
};
static MP_DEFINE_CONST_DICT(mp_module_scd41_globals, scd41_globals_table);

/***** Module Definition *****/
const mp_obj_module_t scd41_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_scd41_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_scd41, scd41_user_cmodule, MODULE_BREAKOUT_SCD41_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_scd41, scd41_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
