#include "pico_explorer.h"

/***** Constants *****/
enum buttons
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
};

enum adcs
{
    ADC0 = 0,
    ADC1,
    ADC2,
};

enum motors
{
    MOTOR1 = 0,
    MOTOR2,
};

enum motions
{
    FORWARD = 0,
    REVERSE,
    STOP,
};

enum gps
{
    GP0 = 0,
    GP1,
    GP2,
    GP3,
    GP4,
    GP5,
    GP6,
    GP7,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picoexplorer Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoexplorer_init_obj, picoexplorer_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoexplorer_get_width_obj, picoexplorer_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoexplorer_get_height_obj, picoexplorer_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoexplorer_update_obj, picoexplorer_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoexplorer_is_pressed_obj, picoexplorer_is_pressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoexplorer_get_adc_obj, picoexplorer_get_adc);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_set_motor_obj, 2, 3, picoexplorer_set_motor);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoexplorer_set_audio_pin_obj, picoexplorer_set_audio_pin);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_set_tone_obj, 1, 2, picoexplorer_set_tone);

//From PicoGraphics parent class
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_set_pen_obj, 1, 3, picoexplorer_set_pen);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoexplorer_create_pen_obj, picoexplorer_create_pen);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_set_clip_obj, 4, 4, picoexplorer_set_clip);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoexplorer_remove_clip_obj, picoexplorer_remove_clip);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoexplorer_clear_obj, picoexplorer_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picoexplorer_pixel_obj, picoexplorer_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoexplorer_pixel_span_obj, picoexplorer_pixel_span);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_rectangle_obj, 4, 4, picoexplorer_rectangle);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoexplorer_circle_obj, picoexplorer_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_character_obj, 3, 4, picoexplorer_character);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoexplorer_text_obj, 4, 5, picoexplorer_text);


/***** Globals Table *****/
STATIC const mp_map_elem_t picoexplorer_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picoexplorer) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picoexplorer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picoexplorer_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picoexplorer_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picoexplorer_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picoexplorer_is_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_adc), MP_ROM_PTR(&picoexplorer_get_adc_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_motor), MP_ROM_PTR(&picoexplorer_set_motor_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_audio_pin), MP_ROM_PTR(&picoexplorer_set_audio_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_tone), MP_ROM_PTR(&picoexplorer_set_tone_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&picoexplorer_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&picoexplorer_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&picoexplorer_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&picoexplorer_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picoexplorer_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&picoexplorer_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&picoexplorer_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&picoexplorer_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&picoexplorer_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&picoexplorer_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&picoexplorer_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },

    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_INT(ADC0) },
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_INT(ADC1) },
    { MP_ROM_QSTR(MP_QSTR_ADC2), MP_ROM_INT(ADC2) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR1), MP_ROM_INT(MOTOR1) },
    { MP_ROM_QSTR(MP_QSTR_MOTOR2), MP_ROM_INT(MOTOR2) },
    { MP_ROM_QSTR(MP_QSTR_FORWARD), MP_ROM_INT(FORWARD) },
    { MP_ROM_QSTR(MP_QSTR_REVERSE), MP_ROM_INT(REVERSE) },
    { MP_ROM_QSTR(MP_QSTR_STOP), MP_ROM_INT(STOP) },
    { MP_ROM_QSTR(MP_QSTR_GP0), MP_ROM_INT(GP0) },
    { MP_ROM_QSTR(MP_QSTR_GP1), MP_ROM_INT(GP1) },
    { MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_INT(GP2) },
    { MP_ROM_QSTR(MP_QSTR_GP3), MP_ROM_INT(GP3) },
    { MP_ROM_QSTR(MP_QSTR_GP4), MP_ROM_INT(GP4) },
    { MP_ROM_QSTR(MP_QSTR_GP5), MP_ROM_INT(GP5) },
    { MP_ROM_QSTR(MP_QSTR_GP6), MP_ROM_INT(GP6) },
    { MP_ROM_QSTR(MP_QSTR_GP7), MP_ROM_INT(GP7) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picoexplorer_globals, picoexplorer_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picoexplorer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picoexplorer_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picoexplorer, picoexplorer_user_cmodule, MODULE_PICOEXPLORER_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////