#include "breakout_encoder_wheel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutEncoderWheel Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_ioe_address_obj, 2, BreakoutEncoderWheel_set_ioe_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_get_interrupt_flag_obj, BreakoutEncoderWheel_get_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_clear_interrupt_flag_obj, BreakoutEncoderWheel_clear_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutEncoderWheel_pressed_obj, BreakoutEncoderWheel_pressed);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_count_obj, BreakoutEncoderWheel_count);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_delta_obj, BreakoutEncoderWheel_delta);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_step_obj, BreakoutEncoderWheel_step);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_turn_obj, BreakoutEncoderWheel_turn);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_zero_obj, BreakoutEncoderWheel_zero);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_revolutions_obj, BreakoutEncoderWheel_revolutions);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_degrees_obj, BreakoutEncoderWheel_degrees);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_radians_obj, BreakoutEncoderWheel_radians);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_direction_obj, 1, BreakoutEncoderWheel_direction);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_rgb_obj, 2, BreakoutEncoderWheel_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_set_hsv_obj, 2, BreakoutEncoderWheel_set_hsv);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_clear_obj, BreakoutEncoderWheel_clear);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutEncoderWheel_show_obj, BreakoutEncoderWheel_show);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_gpio_pin_mode_obj, 2, BreakoutEncoderWheel_gpio_pin_mode);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_gpio_pin_value_obj, 2, BreakoutEncoderWheel_gpio_pin_value);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_gpio_pwm_load_obj, 1, BreakoutEncoderWheel_gpio_pwm_load);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutEncoderWheel_gpio_pwm_frequency_obj, 2, BreakoutEncoderWheel_gpio_pwm_frequency);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutEncoderWheel_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_ioe_address), MP_ROM_PTR(&BreakoutEncoderWheel_set_ioe_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_interrupt_flag), MP_ROM_PTR(&BreakoutEncoderWheel_get_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupt_flag), MP_ROM_PTR(&BreakoutEncoderWheel_clear_interrupt_flag_obj) },

    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&BreakoutEncoderWheel_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&BreakoutEncoderWheel_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_delta), MP_ROM_PTR(&BreakoutEncoderWheel_delta_obj) },
    { MP_ROM_QSTR(MP_QSTR_step), MP_ROM_PTR(&BreakoutEncoderWheel_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_turn), MP_ROM_PTR(&BreakoutEncoderWheel_turn_obj) },
    { MP_ROM_QSTR(MP_QSTR_zero), MP_ROM_PTR(&BreakoutEncoderWheel_zero_obj) },
    { MP_ROM_QSTR(MP_QSTR_revolutions), MP_ROM_PTR(&BreakoutEncoderWheel_revolutions_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees), MP_ROM_PTR(&BreakoutEncoderWheel_degrees_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians), MP_ROM_PTR(&BreakoutEncoderWheel_radians_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&BreakoutEncoderWheel_direction_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&BreakoutEncoderWheel_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&BreakoutEncoderWheel_set_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutEncoderWheel_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&BreakoutEncoderWheel_show_obj) },

    { MP_ROM_QSTR(MP_QSTR_gpio_pin_mode), MP_ROM_PTR(&BreakoutEncoderWheel_gpio_pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio_pin_value), MP_ROM_PTR(&BreakoutEncoderWheel_gpio_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio_pwm_load), MP_ROM_PTR(&BreakoutEncoderWheel_gpio_pwm_load_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio_pwm_frequency), MP_ROM_PTR(&BreakoutEncoderWheel_gpio_pwm_frequency_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutEncoderWheel_locals_dict, BreakoutEncoderWheel_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_encoder_wheel_BreakoutEncoderWheel_type,
    MP_QSTR_BreakoutEncoderWheel,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutEncoderWheel_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutEncoderWheel_locals_dict
);
#else
const mp_obj_type_t breakout_encoder_wheel_BreakoutEncoderWheel_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutEncoderWheel,
    .make_new = BreakoutEncoderWheel_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutEncoderWheel_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_encoder_wheel Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Constants *****/
const mp_rom_obj_tuple_t breakout_encoder_wheel_gpio_pins = {
    {&mp_type_tuple}, 3, { MP_ROM_INT(7), MP_ROM_INT(8), MP_ROM_INT(9) },
};

/***** Globals Table *****/
STATIC const mp_rom_map_elem_t breakout_encoder_wheel_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_encoder_wheel) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutEncoderWheel), (mp_obj_t)&breakout_encoder_wheel_BreakoutEncoderWheel_type },

    { MP_ROM_QSTR(MP_QSTR_DEFAULT_IOE_I2C_ADDR), MP_ROM_INT(0x13) },
    { MP_ROM_QSTR(MP_QSTR_DEFAULT_LED_I2C_ADDR), MP_ROM_INT(0x77) },
    { MP_ROM_QSTR(MP_QSTR_ALTERNATE_LED_I2C_ADDR), MP_ROM_INT(0x74) },

    { MP_ROM_QSTR(MP_QSTR_NUM_LEDS), MP_ROM_INT(24) },
    { MP_ROM_QSTR(MP_QSTR_NUM_BUTTONS), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_NUM_GPIOS), MP_ROM_INT(3) },

    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_CENTRE), MP_ROM_INT(4) },

    { MP_ROM_QSTR(MP_QSTR_GP7), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_GP8), MP_ROM_INT(8) },
    { MP_ROM_QSTR(MP_QSTR_GP9), MP_ROM_INT(9) },
    { MP_ROM_QSTR(MP_QSTR_GPIOS), MP_ROM_PTR(&breakout_encoder_wheel_gpio_pins) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_encoder_wheel_globals, breakout_encoder_wheel_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_encoder_wheel_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_encoder_wheel_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder_wheel, breakout_encoder_wheel_user_cmodule, MODULE_BREAKOUT_ENCODER_WHEEL_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_encoder_wheel, breakout_encoder_wheel_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////