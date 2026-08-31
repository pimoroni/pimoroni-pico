#include "servo_cluster.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster___del___obj, ServoCluster___del__);
MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster_count_obj, ServoCluster_count);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_pin_obj, 2, ServoCluster_pin);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_enable_obj, 2, ServoCluster_enable);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_enable_all_obj, 1, ServoCluster_enable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_disable_obj, 2, ServoCluster_disable);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_disable_all_obj, 1, ServoCluster_disable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_is_enabled_obj, 2, ServoCluster_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_pulse_obj, 2, ServoCluster_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_pulse_obj, 1, ServoCluster_all_to_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_value_obj, 2, ServoCluster_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_value_obj, 1, ServoCluster_all_to_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_phase_obj, 2, ServoCluster_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_phase_obj, 1, ServoCluster_all_to_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_frequency_obj, 1, ServoCluster_frequency);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_min_value_obj, 2, ServoCluster_min_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_mid_value_obj, 2, ServoCluster_mid_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_max_value_obj, 2, ServoCluster_max_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_min_obj, 2, ServoCluster_to_min);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_min_obj, 1, ServoCluster_all_to_min);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_mid_obj, 2, ServoCluster_to_mid);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_mid_obj, 1, ServoCluster_all_to_mid);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_max_obj, 2, ServoCluster_to_max);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_max_obj, 1, ServoCluster_all_to_max);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_percent_obj, 3, ServoCluster_to_percent);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_all_to_percent_obj, 2, ServoCluster_all_to_percent);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_calibration_obj, 2, ServoCluster_calibration);
MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster_load_obj, ServoCluster_load);

/***** Binding of Methods *****/
static const mp_rom_map_elem_t ServoCluster_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ServoCluster___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&ServoCluster_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&ServoCluster_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&ServoCluster_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_all), MP_ROM_PTR(&ServoCluster_enable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&ServoCluster_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_all), MP_ROM_PTR(&ServoCluster_disable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&ServoCluster_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse), MP_ROM_PTR(&ServoCluster_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_pulse), MP_ROM_PTR(&ServoCluster_all_to_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&ServoCluster_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_value), MP_ROM_PTR(&ServoCluster_all_to_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&ServoCluster_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_phase), MP_ROM_PTR(&ServoCluster_all_to_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&ServoCluster_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_value), MP_ROM_PTR(&ServoCluster_min_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_mid_value), MP_ROM_PTR(&ServoCluster_mid_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_value), MP_ROM_PTR(&ServoCluster_max_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_min), MP_ROM_PTR(&ServoCluster_to_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_min), MP_ROM_PTR(&ServoCluster_all_to_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_mid), MP_ROM_PTR(&ServoCluster_to_mid_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_mid), MP_ROM_PTR(&ServoCluster_all_to_mid_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_max), MP_ROM_PTR(&ServoCluster_to_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_max), MP_ROM_PTR(&ServoCluster_all_to_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&ServoCluster_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_percent), MP_ROM_PTR(&ServoCluster_all_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&ServoCluster_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&ServoCluster_load_obj) },
};

static MP_DEFINE_CONST_DICT(ServoCluster_locals_dict, ServoCluster_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    ServoCluster_type,
    MP_QSTR_servo_cluster,
    MP_TYPE_FLAG_NONE,
    make_new, ServoCluster_make_new,
    print, ServoCluster_print,
    locals_dict, (mp_obj_dict_t*)&ServoCluster_locals_dict
);
#else
const mp_obj_type_t ServoCluster_type = {
    { &mp_type_type },
    .name = MP_QSTR_servo_cluster,
    .print = ServoCluster_print,
    .make_new = ServoCluster_make_new,
    .locals_dict = (mp_obj_dict_t*)&ServoCluster_locals_dict,
};
#endif

// The module attribute is registered in servo.c behind MODULE_SERVO_CLUSTER_ENABLED, which the
// qstr extraction pass does not evaluate, so the name is anchored here instead.
const qstr servo_cluster_module_attr = MP_QSTR_ServoCluster;
