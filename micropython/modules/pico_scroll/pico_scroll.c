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
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll___del___obj, picoscroll___del__);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_get_width_obj, picoscroll_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_get_height_obj, picoscroll_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_show_obj, picoscroll_show);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoscroll_set_pixel_obj, 4, 4, picoscroll_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picoscroll_set_pixels_obj, picoscroll_set_pixels);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoscroll_show_text_obj, 4, 4, picoscroll_show_text);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoscroll_scroll_text_obj, 4, 4, picoscroll_scroll_text);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picoscroll_show_bitmap_1d_obj, 4, 4, picoscroll_show_bitmap_1d);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picoscroll_clear_obj, picoscroll_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picoscroll_is_pressed_obj, picoscroll_is_pressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picoscroll_update_obj, picoscroll_update);

/* Class Methods */
STATIC const mp_rom_map_elem_t picoscroll_locals[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&picoscroll___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picoscroll_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&picoscroll_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picoscroll_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picoscroll_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&picoscroll_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixels), MP_ROM_PTR(&picoscroll_set_pixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_text), MP_ROM_PTR(&picoscroll_show_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll_text), MP_ROM_PTR(&picoscroll_scroll_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_bitmap_1d), MP_ROM_PTR(&picoscroll_show_bitmap_1d_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picoscroll_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picoscroll_is_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picoscroll_locals, picoscroll_locals);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    PicoScroll_type,
    MP_QSTR_PicoScroll,
    MP_TYPE_FLAG_NONE,
    make_new, picoscroll_make_new,
    locals_dict, (mp_obj_dict_t*)&mp_module_picoscroll_locals
);
#else
const mp_obj_type_t PicoScroll_type = {
    { &mp_type_type },
    .name = MP_QSTR_PicoScroll,
    .make_new = picoscroll_make_new,
    .locals_dict = (mp_obj_dict_t*)&mp_module_picoscroll_locals,
};
#endif

/* Module Globals */
STATIC const mp_map_elem_t picoscroll_globals[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_picoscroll) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PicoScroll), (mp_obj_t)&PicoScroll_type },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picoscroll_globals, picoscroll_globals);

const mp_obj_module_t picoscroll_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picoscroll_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_picoscroll, picoscroll_user_cmodule, MODULE_PICOSCROLL_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_picoscroll, picoscroll_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
