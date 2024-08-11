#include "servo.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Calibration___del___obj, Calibration___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_apply_blank_pairs_obj, 2, Calibration_apply_blank_pairs);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_apply_two_pairs_obj, 5, Calibration_apply_two_pairs);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_apply_three_pairs_obj, 7, Calibration_apply_three_pairs);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_apply_uniform_pairs_obj, 6, Calibration_apply_uniform_pairs);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_apply_default_pairs_obj, 2, Calibration_apply_default_pairs);
MP_DEFINE_CONST_FUN_OBJ_1(Calibration_size_obj, Calibration_size);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_pair_obj, 2, Calibration_pair);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_pulse_obj, 2, Calibration_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_value_obj, 2, Calibration_value);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_first_obj, 1, Calibration_first);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_first_pulse_obj, 1, Calibration_first_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_first_value_obj, 1, Calibration_first_value);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_last_obj, 1, Calibration_last);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_last_pulse_obj, 1, Calibration_last_pulse);
MP_DEFINE_CONST_FUN_OBJ_KW(Calibration_last_value_obj, 1, Calibration_last_value);
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
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_calibration_obj, 1, Servo_calibration);

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
STATIC const mp_rom_map_elem_t Calibration_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Calibration___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_blank_pairs), MP_ROM_PTR(&Calibration_apply_blank_pairs_obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_two_pairs), MP_ROM_PTR(&Calibration_apply_two_pairs_obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_three_pairs), MP_ROM_PTR(&Calibration_apply_three_pairs_obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_uniform_pairs), MP_ROM_PTR(&Calibration_apply_uniform_pairs_obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_default_pairs), MP_ROM_PTR(&Calibration_apply_default_pairs_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&Calibration_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_pair), MP_ROM_PTR(&Calibration_pair_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse), MP_ROM_PTR(&Calibration_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&Calibration_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_first), MP_ROM_PTR(&Calibration_first_obj) },
    { MP_ROM_QSTR(MP_QSTR_first_pulse), MP_ROM_PTR(&Calibration_first_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_first_value), MP_ROM_PTR(&Calibration_first_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_last), MP_ROM_PTR(&Calibration_last_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_pulse), MP_ROM_PTR(&Calibration_last_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_value), MP_ROM_PTR(&Calibration_last_value_obj) },
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

STATIC MP_DEFINE_CONST_DICT(Calibration_locals_dict, Calibration_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(Servo_locals_dict, Servo_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(ServoCluster_locals_dict, ServoCluster_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    Calibration_type,
    MP_QSTR_calibration,
    MP_TYPE_FLAG_NONE,
    make_new, Calibration_make_new,
    print, Calibration_print,
    locals_dict, (mp_obj_dict_t*)&Calibration_locals_dict
);
#else
const mp_obj_type_t Calibration_type = {
    { &mp_type_type },
    .name = MP_QSTR_calibration,
    .print = Calibration_print,
    .make_new = Calibration_make_new,
    .locals_dict = (mp_obj_dict_t*)&Calibration_locals_dict,
};
#endif

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    Servo_type,
    MP_QSTR_servo,
    MP_TYPE_FLAG_NONE,
    make_new, Servo_make_new,
    print, Servo_print,
    locals_dict, (mp_obj_dict_t*)&Servo_locals_dict
);
#else
const mp_obj_type_t Servo_type = {
    { &mp_type_type },
    .name = MP_QSTR_servo,
    .print = Servo_print,
    .make_new = Servo_make_new,
    .locals_dict = (mp_obj_dict_t*)&Servo_locals_dict,
};
#endif

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

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
mp_obj_float_t servo2040_shunt_resistor = {{&mp_type_float}, 0.003f};
mp_obj_float_t servo2040_voltage_gain = {{&mp_type_float}, 3.9f / 13.9f};
mp_obj_float_t servo2040_current_offset = {{&mp_type_float}, -0.02f};

/***** Globals Table *****/
STATIC const mp_map_elem_t servo2040_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_servo2040) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_1), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_2), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_3), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_4), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_5), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_6), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_7), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_8), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_9), MP_ROM_INT(8) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_10), MP_ROM_INT(9) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_11), MP_ROM_INT(10) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_12), MP_ROM_INT(11) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_13), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_14), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_15), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_16), MP_ROM_INT(15) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_17), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_SERVO_18), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_NUM_SERVOS), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_LED_DATA), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_NUM_LEDS), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_INT), MP_ROM_INT(19) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_INT(20) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_INT(21) },
    { MP_ROM_QSTR(MP_QSTR_USER_SW), MP_ROM_INT(23) },
    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_0), MP_ROM_INT(22) },
    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_1), MP_ROM_INT(24) },
    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_2), MP_ROM_INT(25) },
    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_INT(26) },
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_ADC2), MP_ROM_INT(28) },
    { MP_ROM_QSTR(MP_QSTR_SHARED_ADC), MP_ROM_INT(29) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_1_ADDR), MP_ROM_INT(0b000) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_2_ADDR), MP_ROM_INT(0b001) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_3_ADDR), MP_ROM_INT(0b010) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_4_ADDR), MP_ROM_INT(0b011) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_5_ADDR), MP_ROM_INT(0b100) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_6_ADDR), MP_ROM_INT(0b101) },
    { MP_ROM_QSTR(MP_QSTR_NUM_SENSORS), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_SENSE_ADDR), MP_ROM_INT(0b110) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_ADDR), MP_ROM_INT(0b111) },
    { MP_ROM_QSTR(MP_QSTR_SHUNT_RESISTOR), MP_ROM_PTR(&servo2040_shunt_resistor) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_GAIN), MP_ROM_INT(69) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_GAIN), MP_ROM_PTR(&servo2040_voltage_gain) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_OFFSET), MP_ROM_PTR(&servo2040_current_offset) },
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
};
STATIC MP_DEFINE_CONST_DICT(mp_module_servo_globals, servo_globals_table);

/***** Module Definition *****/
const mp_obj_module_t servo_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_servo_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_servo, servo_user_cmodule, MODULE_SERVO_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_servo, servo_user_cmodule);
#endif
