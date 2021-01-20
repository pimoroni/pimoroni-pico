#include "pico_display.h"

/***** Constants *****/
enum buttons
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picodisplay Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_init_obj, picodisplay_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_get_width_obj, picodisplay_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_get_height_obj, picodisplay_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_update_obj, picodisplay_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_set_backlight_obj, picodisplay_set_backlight);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picodisplay_set_led_obj, picodisplay_set_led);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_is_pressed_obj, picodisplay_is_pressed);

//From PicoGraphics parent class
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picodisplay_set_pen_rgb_obj, picodisplay_set_pen_rgb);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_set_pen_obj, picodisplay_set_pen);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picodisplay_create_pen_obj, picodisplay_create_pen);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_set_clip_obj, picodisplay_set_clip);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_remove_clip_obj, picodisplay_remove_clip);
//STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_ptr_obj, picodisplay_ptr);
//STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_ptr_obj, picodisplay_ptr);
//STATIC MP_DEFINE_CONST_FUN_OBJ_2(picodisplay_ptr_obj, picodisplay_ptr);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picodisplay_clear_obj, picodisplay_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_pixel_obj, picodisplay_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picodisplay_pixel_span_obj, picodisplay_pixel_span);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodisplay_rectangle_obj, picodisplay_rectangle);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picodisplay_circle_obj, picodisplay_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picodisplay_text_obj, picodisplay_text);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picodisplay_character_obj, picodisplay_character);


/***** Globals Table *****/
STATIC const mp_map_elem_t picodisplay_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picodisplay) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picodisplay_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picodisplay_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picodisplay_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picodisplay_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&picodisplay_set_backlight_obj) },    
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&picodisplay_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picodisplay_is_pressed_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_pen_rgb), MP_ROM_PTR(&picodisplay_set_pen_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&picodisplay_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&picodisplay_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&picodisplay_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&picodisplay_remove_clip_obj) },
    //
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picodisplay_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&picodisplay_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&picodisplay_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&picodisplay_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&picodisplay_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&picodisplay_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&picodisplay_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picodisplay_globals, picodisplay_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picodisplay_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picodisplay_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picodisplay, picodisplay_user_cmodule, MODULE_PICODISPLAY_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////