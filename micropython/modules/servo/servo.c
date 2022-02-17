#include "servo.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Servo___del___obj, Servo___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_enable_obj, Servo_enable);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_disable_obj, Servo_disable);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_is_enabled_obj, Servo_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_value_obj, 1, Servo_value);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_pulse_obj, 1, Servo_pulse);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_min_obj, Servo_to_min);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_mid_obj, Servo_to_mid);
MP_DEFINE_CONST_FUN_OBJ_1(Servo_to_max_obj, Servo_to_max);
MP_DEFINE_CONST_FUN_OBJ_KW(Servo_to_percent_obj, 1, Servo_to_percent);

MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster___del___obj, ServoCluster___del__);
// MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_set_rgb_obj, 5, ServoCluster_set_rgb);
// MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_set_hsv_obj, 3, ServoCluster_set_hsv);
// MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_set_brightness_obj, 2, ServoCluster_set_brightness);
// MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_start_obj, 1, ServoCluster_start);
// MP_DEFINE_CONST_FUN_OBJ_KW(ServoCluster_get_obj, 2, ServoCluster_get);
// MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster_clear_obj, ServoCluster_clear);
// MP_DEFINE_CONST_FUN_OBJ_1(ServoCluster_update_obj, ServoCluster_update);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Servo_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Servo___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&Servo_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&Servo_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&Servo_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&Servo_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse), MP_ROM_PTR(&Servo_pulse_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_min), MP_ROM_PTR(&Servo_to_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_mid), MP_ROM_PTR(&Servo_to_mid_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_max), MP_ROM_PTR(&Servo_to_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_percent), MP_ROM_PTR(&Servo_to_percent_obj) },
};

STATIC const mp_rom_map_elem_t ServoCluster_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ServoCluster___del___obj) },
    // { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&ServoCluster_set_rgb_obj) },
    // { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&ServoCluster_set_hsv_obj) },
    // { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&ServoCluster_set_brightness_obj) },
    // { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&ServoCluster_start_obj) },
    // { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&ServoCluster_get_obj) },
    // { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&ServoCluster_clear_obj) },
    // { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&ServoCluster_update_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Servo_locals_dict, Servo_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(ServoCluster_locals_dict, ServoCluster_locals_dict_table);

/***** Class Definition *****/
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
MP_REGISTER_MODULE(MP_QSTR_servo, servo_user_cmodule, MODULE_SERVO_ENABLED);
