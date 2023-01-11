#include "motor.h"

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Motor___del___obj, Motor___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_pins_obj, Motor_pins);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_enable_obj, Motor_enable);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_disable_obj, Motor_disable);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_is_enabled_obj, Motor_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_duty_obj, 1, Motor_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_speed_obj, 1, Motor_speed);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_frequency_obj, 1, Motor_frequency);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_stop_obj, Motor_stop);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_coast_obj, Motor_coast);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_brake_obj, Motor_brake);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_full_negative_obj, Motor_full_negative);
MP_DEFINE_CONST_FUN_OBJ_1(Motor_full_positive_obj, Motor_full_positive);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_to_percent_obj, 2, Motor_to_percent);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_direction_obj, 1, Motor_direction);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_speed_scale_obj, 1, Motor_speed_scale);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_zeropoint_obj, 1, Motor_zeropoint);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_deadzone_obj, 1, Motor_deadzone);
MP_DEFINE_CONST_FUN_OBJ_KW(Motor_decay_mode_obj, 1, Motor_decay_mode);

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
STATIC const mp_rom_map_elem_t Motor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Motor___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&Motor_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&Motor_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&Motor_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&Motor_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&Motor_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_speed), MP_ROM_PTR(&Motor_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Motor_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&Motor_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_coast), MP_ROM_PTR(&Motor_coast_obj) },
    { MP_ROM_QSTR(MP_QSTR_brake), MP_ROM_PTR(&Motor_brake_obj) },
    { MP_ROM_QSTR(MP_QSTR_full_negative), MP_ROM_PTR(&Motor_full_negative_obj) },
    { MP_ROM_QSTR(MP_QSTR_full_positive), MP_ROM_PTR(&Motor_full_positive_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&Motor_to_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&Motor_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_speed_scale), MP_ROM_PTR(&Motor_speed_scale_obj) },
    { MP_ROM_QSTR(MP_QSTR_zeropoint), MP_ROM_PTR(&Motor_zeropoint_obj) },
    { MP_ROM_QSTR(MP_QSTR_deadzone), MP_ROM_PTR(&Motor_deadzone_obj) },
    { MP_ROM_QSTR(MP_QSTR_decay_mode), MP_ROM_PTR(&Motor_decay_mode_obj) },
};

STATIC const mp_rom_map_elem_t MotorCluster_locals_dict_table[] = {
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

STATIC MP_DEFINE_CONST_DICT(Motor_locals_dict, Motor_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(MotorCluster_locals_dict, MotorCluster_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    Motor_type,
    MP_QSTR_motor,
    MP_TYPE_FLAG_NONE,
    make_new, Motor_make_new,
    print, Motor_print,
    locals_dict, (mp_obj_dict_t*)&Motor_locals_dict
);
#else
const mp_obj_type_t Motor_type = {
    { &mp_type_type },
    .name = MP_QSTR_motor,
    .print = Motor_print,
    .make_new = Motor_make_new,
    .locals_dict = (mp_obj_dict_t*)&Motor_locals_dict,
};
#endif

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


/***** Module Constants *****/
const mp_rom_obj_tuple_t pico_motor_shim_motor1_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(6), MP_ROM_INT(7), },
};
const mp_rom_obj_tuple_t pico_motor_shim_motor2_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(27), MP_ROM_INT(26), },
};

const mp_rom_obj_tuple_t motor2040_motorA_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(4), MP_ROM_INT(5) },
};
const mp_rom_obj_tuple_t motor2040_motorB_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(6), MP_ROM_INT(7) },
};
const mp_rom_obj_tuple_t motor2040_motorC_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(8), MP_ROM_INT(9) },
};
const mp_rom_obj_tuple_t motor2040_motorD_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(10), MP_ROM_INT(11) },
};
const mp_rom_obj_tuple_t motor2040_encoderA_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(1), MP_ROM_INT(0) },
};
const mp_rom_obj_tuple_t motor2040_encoderB_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(3), MP_ROM_INT(2) },
};
const mp_rom_obj_tuple_t motor2040_encoderC_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(13), MP_ROM_INT(12) },
};
const mp_rom_obj_tuple_t motor2040_encoderD_pins = {
    {&mp_type_tuple}, 2, { MP_ROM_INT(15), MP_ROM_INT(14) },
};

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
mp_obj_float_t motor2040_shunt_resistor = {{&mp_type_float}, 0.47f};
mp_obj_float_t motor2040_voltage_gain = {{&mp_type_float}, 3.9f / 13.9f};
mp_obj_float_t motor2040_current_offset = {{&mp_type_float}, -0.005f};


/***** Globals Table *****/
STATIC const mp_rom_map_elem_t pico_motor_shim_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pico_motor_shim) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_1), MP_ROM_PTR(&pico_motor_shim_motor1_pins) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_2), MP_ROM_PTR(&pico_motor_shim_motor2_pins) },
    { MP_ROM_QSTR(MP_QSTR_NUM_MOTORS), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_INT(5) },
};

STATIC const mp_rom_map_elem_t motor2040_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_motor2040) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_A), MP_ROM_PTR(&motor2040_motorA_pins) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_B), MP_ROM_PTR(&motor2040_motorB_pins) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_C), MP_ROM_PTR(&motor2040_motorC_pins) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR_D), MP_ROM_PTR(&motor2040_motorD_pins) },
    { MP_ROM_QSTR(MP_QSTR_NUM_MOTORS), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_ENCODER_A), MP_ROM_PTR(&motor2040_encoderA_pins) },
    { MP_ROM_QSTR(MP_QSTR_ENCODER_B), MP_ROM_PTR(&motor2040_encoderB_pins) },
    { MP_ROM_QSTR(MP_QSTR_ENCODER_C), MP_ROM_PTR(&motor2040_encoderC_pins) },
    { MP_ROM_QSTR(MP_QSTR_ENCODER_D), MP_ROM_PTR(&motor2040_encoderD_pins) },
    { MP_ROM_QSTR(MP_QSTR_NUM_ENCODERS), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_TX_TRIG), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_RX_ECHO), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_LED_DATA), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_NUM_LEDS), MP_ROM_INT(1) },
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
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_A_ADDR), MP_ROM_INT(0b000) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_B_ADDR), MP_ROM_INT(0b001) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_C_ADDR), MP_ROM_INT(0b010) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_D_ADDR), MP_ROM_INT(0b011) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_SENSE_ADDR), MP_ROM_INT(0b100) },
    { MP_ROM_QSTR(MP_QSTR_FAULT_SENSE_ADDR), MP_ROM_INT(0b101) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_1_ADDR), MP_ROM_INT(0b110) },
    { MP_ROM_QSTR(MP_QSTR_SENSOR_2_ADDR), MP_ROM_INT(0b111) },
    { MP_ROM_QSTR(MP_QSTR_NUM_SENSORS), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_SHUNT_RESISTOR), MP_ROM_PTR(&motor2040_shunt_resistor) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_GAIN), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_GAIN), MP_ROM_PTR(&motor2040_voltage_gain) },
    { MP_ROM_QSTR(MP_QSTR_CURRENT_OFFSET), MP_ROM_PTR(&motor2040_current_offset) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_pico_motor_shim_globals, pico_motor_shim_globals_table);
STATIC MP_DEFINE_CONST_DICT(mp_module_motor2040_globals, motor2040_globals_table);

const mp_obj_module_t pico_motor_shim_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_pico_motor_shim_globals,
};

const mp_obj_module_t motor2040_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_motor2040_globals,
};

STATIC const mp_map_elem_t motor_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_motor) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Motor), (mp_obj_t)&Motor_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MotorCluster), (mp_obj_t)&MotorCluster_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pico_motor_shim), (mp_obj_t)&pico_motor_shim_user_cmodule },
    { MP_OBJ_NEW_QSTR(MP_QSTR_motor2040), (mp_obj_t)&motor2040_user_cmodule },

    { MP_ROM_QSTR(MP_QSTR_FAST_DECAY), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_SLOW_DECAY), MP_ROM_INT(0x01) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_motor_globals, motor_globals_table);

/***** Module Definition *****/
const mp_obj_module_t motor_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_motor_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_motor, motor_user_cmodule, MODULE_MOTOR_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_motor, motor_user_cmodule);
#endif
