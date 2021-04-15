#include "pico_scroll.h"

/***** Constants *****/
enum buttons
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picoscroll Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_init_obj, picoscroll_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_get_width_obj, picoscroll_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_get_height_obj, picoscroll_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_update_obj, picoscroll_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoscroll_set_pixel_obj, picoscroll_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_set_pixels_obj, picoscroll_set_pixels);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoscroll_show_text_obj, picoscroll_show_text);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picoscroll_show_bitmap_1d_obj, picoscroll_show_bitmap_1d);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picoscroll_clear_obj, picoscroll_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_is_pressed_obj, picoscroll_is_pressed);

/***** Globals Table *****/
STATIC const mp_map_elem_t picoscroll_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picoscroll) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picoscroll_init_obj) },    
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picoscroll_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picoscroll_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picoscroll_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&picoscroll_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixels), MP_ROM_PTR(&picoscroll_set_pixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_text), MP_ROM_PTR(&picoscroll_show_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_bitmap_1d), MP_ROM_PTR(&picoscroll_show_bitmap_1d_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picoscroll_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picoscroll_is_pressed_obj) },        
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picoscroll_globals, picoscroll_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picoscroll_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picoscroll_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picoscroll, picoscroll_user_cmodule, MODULE_PICOSCROLL_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
