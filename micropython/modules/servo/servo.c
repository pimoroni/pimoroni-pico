#include "servo.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Calibration___del___obj, Calibration___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_create_blank_calibration_obj, 2, Calibration_create_blank_calibration);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_create_two_point_calibration_obj, 5, Calibration_create_two_point_calibration);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_create_three_point_calibration_obj, 7, Calibration_create_three_point_calibration);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_create_uniform_calibration_obj, 6, Calibration_create_uniform_calibration);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_create_default_calibration_obj, 2, Calibration_create_default_calibration);
MP_DEFINE_CONST_FUN_OBJ_1(Calibration_size_obj, Calibration_size);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_point_at_obj, 2, Calibration_point_at);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_first_point_obj, 1, Calibration_first_point);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_last_point_obj, 1, Calibration_last_point);
MP_DEFINE_CONST_FUN_OBJ_1(Calibration_has_lower_limit_obj, Calibration_has_lower_limit);
MP_DEFINE_CONST_FUN_OBJ_1(Calibration_has_upper_limit_obj, Calibration_has_upper_limit);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_limit_to_calibration_obj, 3, Calibration_limit_to_calibration);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_value_to_pulse_obj, 2, Calibration_value_to_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_pulse_to_value_obj, 2, Calibration_pulse_to_value);

MP_DEFINE_CONST_FUN_OBJ_1(Servo___del___obj, Servo___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_pin_obj, Servo_pin);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_enable_obj, Servo_enable);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_disable_obj, Servo_disable);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_is_enabled_obj, Servo_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_value_obj, 1, Servo_value);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_pulse_obj, 1, Servo_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_frequency_obj, 1, Servo_frequency);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_min_value_obj, Servo_min_value);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_mid_value_obj, Servo_mid_value);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_max_value_obj, Servo_max_value);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_min_obj, Servo_to_min);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_mid_obj, Servo_to_mid);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_max_obj, Servo_to_max);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_to_percent_obj, 2, Servo_to_percent);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_calibration_obj, Servo_calibration);

MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster___del___obj, ServoCluster___del__);
MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster_count_obj, ServoCluster_count);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_pin_obj, 2, ServoCluster_pin);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_enable_obj, 2, ServoCluster_enable);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_disable_obj, 2, ServoCluster_disable);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_is_enabled_obj, 2, ServoCluster_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_value_obj, 2, ServoCluster_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_pulse_obj, 2, ServoCluster_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_phase_obj, 2, ServoCluster_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_frequency_obj, 1, ServoCluster_frequency);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_min_value_obj, 2, ServoCluster_min_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_mid_value_obj, 2, ServoCluster_mid_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_max_value_obj, 2, ServoCluster_max_value);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_min_obj, 2, ServoCluster_to_min);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_mid_obj, 2, ServoCluster_to_mid);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_max_obj, 2, ServoCluster_to_max);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_to_percent_obj, 3, ServoCluster_to_percent);
MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_calibration_obj, 2, ServoCluster_calibration);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Calibration_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Calibration___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_create_blank_calibration), MP_ROM_PTR(&Calibration_create_blank_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_two_point_calibration), MP_ROM_PTR(&Calibration_create_two_point_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_three_point_calibration), MP_ROM_PTR(&Calibration_create_three_point_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_uniform_calibration), MP_ROM_PTR(&Calibration_create_uniform_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_default_calibration), MP_ROM_PTR(&Calibration_create_default_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&Calibration_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_point_at), MP_ROM_PTR(&Calibration_point_at_obj) },
    { MP_ROM_QSTR(MP_QSTR_first_point), MP_ROM_PTR(&Calibration_first_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_point), MP_ROM_PTR(&Calibration_last_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_has_lower_limit), MP_ROM_PTR(&Calibration_has_lower_limit_obj) },
    { MP_ROM_QSTR(MP_QSTR_has_upper_limit), MP_ROM_PTR(&Calibration_has_upper_limit_obj) },
    { MP_ROM_QSTR(MP_QSTR_limit_to_calibration), MP_ROM_PTR(&Calibration_limit_to_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_value_to_pulse), MP_ROM_PTR(&Calibration_value_to_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse_to_value), MP_ROM_PTR(&Calibration_pulse_to_value_obj) },
};

STATIC const mp_rom_map_elem_t Servo_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Servo___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&Servo_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&Servo_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&Servo_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&Servo_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse), MP_ROM_PTR(&Servo_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&Servo_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Servo_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_value), MP_ROM_PTR(&Servo_min_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_mid_value), MP_ROM_PTR(&Servo_mid_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_value), MP_ROM_PTR(&Servo_max_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_min), MP_ROM_PTR(&Servo_to_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_mid), MP_ROM_PTR(&Servo_to_mid_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_max), MP_ROM_PTR(&Servo_to_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&Servo_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&Servo_calibration_obj) },
};

STATIC const mp_rom_map_elem_t ServoCluster_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ServoCluster___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&ServoCluster_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&ServoCluster_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&ServoCluster_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&ServoCluster_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&ServoCluster_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse), MP_ROM_PTR(&ServoCluster_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&ServoCluster_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&ServoCluster_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&ServoCluster_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_value), MP_ROM_PTR(&ServoCluster_min_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_mid_value), MP_ROM_PTR(&ServoCluster_mid_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_value), MP_ROM_PTR(&ServoCluster_max_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_min), MP_ROM_PTR(&ServoCluster_to_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_mid), MP_ROM_PTR(&ServoCluster_to_mid_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_max), MP_ROM_PTR(&ServoCluster_to_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&ServoCluster_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&ServoCluster_calibration_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Calibration_locals_dict, Calibration_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(Servo_locals_dict, Servo_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(ServoCluster_locals_dict, ServoCluster_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t Calibration_type = {
    { &mp_type_type },
    .name = MP_QSTR_calibration,
    .print = Calibration_print,
    .make_new = Calibration_make_new,
    .locals_dict = (mp_obj_dict_t*)&Calibration_locals_dict,
};

const mp_obj_type_t Servo_type = {
    { &mp_type_type },
    .name = MP_QSTR_servo,
    .print = Servo_print,
    .make_new = Servo_make_new,
    .locals_dict = (mp_obj_dict_t*)&Servo_locals_dict,
};

const mp_obj_type_t ServoCluster_type = {
    { &mp_type_type },
    .name = MP_QSTR_servo_cluster,
    .print = ServoCluster_print,
    .make_new = ServoCluster_make_new,
    .locals_dict = (mp_obj_dict_t*)&ServoCluster_locals_dict,
};

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
mp_obj_float_t servo2040_shunt_resistor = {{&mp_type_float}, 0.015f};

/***** Globals Table *****/
STATIC const mp_map_elem_t servo2040_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_servo2040) },
    { MP_ROM_QSTR(MP_QSTR_LED_R), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_LED_G), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_USER_SW), MP_ROM_INT(23) },
    { MP_ROM_QSTR(MP_QSTR_CLK), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_DAT), MP_ROM_INT(15) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE), MP_ROM_INT(29) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_INT(20) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_INT(21) },

    { MP_ROM_QSTR(MP_QSTR_SHUNT_RESISTOR), MP_ROM_PTR(&servo2040_shunt_resistor) },
    { MP_ROM_QSTR(MP_QSTR_ADC_GAIN), MP_ROM_INT(50) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_servo2040_globals, servo2040_globals_table);

const mp_obj_module_t servo2040_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_servo2040_globals,
};

STATIC const mp_map_elem_t servo_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_servo) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Calibration), (mp_obj_t)&Calibration_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Servo), (mp_obj_t)&Servo_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ServoCluster), (mp_obj_t)&ServoCluster_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servo2040), (mp_obj_t)&servo2040_user_cmodule },

    { MP_ROM_QSTR(MP_QSTR_ANGULAR), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_LINEAR), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_CONTINUOUS), MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_EMPTY), MP_ROM_INT(0x03) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_servo_globals, servo_globals_table);

/***** Module Definition *****/
const mp_obj_module_t servo_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_servo_globals,
};
MP_REGISTER_MODULE(MP_QSTR_servo, servo_user_cmodule, MODULE_SERVO_ENABLED);
