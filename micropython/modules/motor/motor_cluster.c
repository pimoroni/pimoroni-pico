#include "motor_cluster.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(MotorCluster___del___obj, MotorCluster___del__);
MP_DEFINE_CONST_FUN_OBJ_1(MotorCluster_count_obj, MotorCluster_count);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_pins_obj, 2, MotorCluster_pins);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_enable_obj, 2, MotorCluster_enable);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_enable_all_obj, 1, MotorCluster_enable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_disable_obj, 2, MotorCluster_disable);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_disable_all_obj, 1, MotorCluster_disable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_is_enabled_obj, 2, MotorCluster_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_duty_obj, 2, MotorCluster_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_to_duty_obj, 1, MotorCluster_all_to_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_speed_obj, 2, MotorCluster_speed);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_to_speed_obj, 1, MotorCluster_all_to_speed);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_phase_obj, 2, MotorCluster_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_to_phase_obj, 1, MotorCluster_all_to_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_frequency_obj, 1, MotorCluster_frequency);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_stop_obj, 2, MotorCluster_stop);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_stop_all_obj, 1, MotorCluster_stop_all);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_coast_obj, 2, MotorCluster_coast);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_coast_all_obj, 1, MotorCluster_coast_all);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_brake_obj, 2, MotorCluster_brake);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_brake_all_obj, 1, MotorCluster_brake_all);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_full_negative_obj, 2, MotorCluster_full_negative);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_full_negative_obj, 1, MotorCluster_all_full_negative);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_full_positive_obj, 2, MotorCluster_full_positive);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_full_positive_obj, 1, MotorCluster_all_full_positive);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_to_percent_obj, 3, MotorCluster_to_percent);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_to_percent_obj, 2, MotorCluster_all_to_percent);
MP_DEFINE_CONST_FUN_OBJ_1(MotorCluster_load_obj, MotorCluster_load);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_direction_obj, 2, MotorCluster_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_directions_obj, 1, MotorCluster_all_directions);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_speed_scale_obj, 2, MotorCluster_speed_scale);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_speed_scales_obj, 1, MotorCluster_all_speed_scales);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_zeropoint_obj, 2, MotorCluster_zeropoint);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_zeropoints_obj, 1, MotorCluster_all_zeropoints);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_deadzone_obj, 2, MotorCluster_deadzone);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_deadzones_obj, 1, MotorCluster_all_deadzones);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_decay_mode_obj, 2, MotorCluster_decay_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(MotorCluster_all_decay_modes_obj, 1, MotorCluster_all_decay_modes);

/***** Binding of Methods *****/
static const mp_rom_map_elem_t MotorCluster_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&MotorCluster___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&MotorCluster_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&MotorCluster_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&MotorCluster_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_all), MP_ROM_PTR(&MotorCluster_enable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&MotorCluster_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_all), MP_ROM_PTR(&MotorCluster_disable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&MotorCluster_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&MotorCluster_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_duty), MP_ROM_PTR(&MotorCluster_all_to_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_speed), MP_ROM_PTR(&MotorCluster_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_speed), MP_ROM_PTR(&MotorCluster_all_to_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&MotorCluster_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_phase), MP_ROM_PTR(&MotorCluster_all_to_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&MotorCluster_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&MotorCluster_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_all), MP_ROM_PTR(&MotorCluster_stop_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_coast), MP_ROM_PTR(&MotorCluster_coast_obj) },
    { MP_ROM_QSTR(MP_QSTR_coast_all), MP_ROM_PTR(&MotorCluster_coast_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_brake), MP_ROM_PTR(&MotorCluster_brake_obj) },
    { MP_ROM_QSTR(MP_QSTR_brake_all), MP_ROM_PTR(&MotorCluster_brake_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_full_negative), MP_ROM_PTR(&MotorCluster_full_negative_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_full_negative), MP_ROM_PTR(&MotorCluster_all_full_negative_obj) },
    { MP_ROM_QSTR(MP_QSTR_full_positive), MP_ROM_PTR(&MotorCluster_full_positive_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_full_positive), MP_ROM_PTR(&MotorCluster_all_full_positive_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&MotorCluster_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_percent), MP_ROM_PTR(&MotorCluster_all_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&MotorCluster_load_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&MotorCluster_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_directions), MP_ROM_PTR(&MotorCluster_all_directions_obj) },
    { MP_ROM_QSTR(MP_QSTR_speed_scale), MP_ROM_PTR(&MotorCluster_speed_scale_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_speed_scales), MP_ROM_PTR(&MotorCluster_all_speed_scales_obj) },
    { MP_ROM_QSTR(MP_QSTR_zeropoint), MP_ROM_PTR(&MotorCluster_zeropoint_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_zeropoints), MP_ROM_PTR(&MotorCluster_all_zeropoints_obj) },
    { MP_ROM_QSTR(MP_QSTR_deadzone), MP_ROM_PTR(&MotorCluster_deadzone_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_deadzones), MP_ROM_PTR(&MotorCluster_all_deadzones_obj) },
    { MP_ROM_QSTR(MP_QSTR_decay_mode), MP_ROM_PTR(&MotorCluster_decay_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_decay_modes), MP_ROM_PTR(&MotorCluster_all_decay_modes_obj) },
};

static MP_DEFINE_CONST_DICT(MotorCluster_locals_dict, MotorCluster_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    MotorCluster_type,
    MP_QSTR_motor_cluster,
    MP_TYPE_FLAG_NONE,
    make_new, MotorCluster_make_new,
    print, MotorCluster_print,
    locals_dict, (mp_obj_dict_t*)&MotorCluster_locals_dict
);
#else
const mp_obj_type_t MotorCluster_type = {
    { &mp_type_type },
    .name = MP_QSTR_motor_cluster,
    .print = MotorCluster_print,
    .make_new = MotorCluster_make_new,
    .locals_dict = (mp_obj_dict_t*)&MotorCluster_locals_dict,
};
#endif

// The module attribute is registered in motor.c behind MODULE_MOTOR_CLUSTER_ENABLED, which the
// qstr extraction pass does not evaluate, so the name is anchored here instead.
const qstr motor_cluster_module_attr = MP_QSTR_MotorCluster;
