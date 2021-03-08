#include "breakout_colourlcd240x240.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutColourLCD240x240 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD240x240_update_obj, BreakoutColourLCD240x240_update);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_set_backlight_obj, 1, BreakoutColourLCD240x240_set_backlight);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_set_pen_obj, 1, BreakoutColourLCD240x240_set_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_create_pen_obj, 1, BreakoutColourLCD240x240_create_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_set_clip_obj, 1, BreakoutColourLCD240x240_set_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD240x240_remove_clip_obj, BreakoutColourLCD240x240_remove_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD240x240_clear_obj, BreakoutColourLCD240x240_clear);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_pixel_obj, 1, BreakoutColourLCD240x240_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_pixel_span_obj, 1, BreakoutColourLCD240x240_pixel_span);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_rectangle_obj, 1, BreakoutColourLCD240x240_rectangle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_circle_obj, 1, BreakoutColourLCD240x240_circle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_character_obj, 1, BreakoutColourLCD240x240_character);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_text_obj, 1, BreakoutColourLCD240x240_text);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_polygon_obj, 1, BreakoutColourLCD240x240_polygon);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_triangle_obj, 1, BreakoutColourLCD240x240_triangle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD240x240_line_obj, 1, BreakoutColourLCD240x240_line);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutColourLCD240x240_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutColourLCD240x240_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&BreakoutColourLCD240x240_set_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&BreakoutColourLCD240x240_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&BreakoutColourLCD240x240_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&BreakoutColourLCD240x240_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&BreakoutColourLCD240x240_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutColourLCD240x240_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&BreakoutColourLCD240x240_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&BreakoutColourLCD240x240_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&BreakoutColourLCD240x240_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&BreakoutColourLCD240x240_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&BreakoutColourLCD240x240_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&BreakoutColourLCD240x240_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon), MP_ROM_PTR(&BreakoutColourLCD240x240_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&BreakoutColourLCD240x240_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&BreakoutColourLCD240x240_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutColourLCD240x240_locals_dict, BreakoutColourLCD240x240_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_colourlcd240x240_BreakoutColourLCD240x240_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_colourlcd240x240,
    .print = BreakoutColourLCD240x240_print,
    .make_new = BreakoutColourLCD240x240_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutColourLCD240x240_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_colourlcd240x240 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_colourlcd240x240_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_colourlcd240x240) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutColourLCD240x240), (mp_obj_t)&breakout_colourlcd240x240_BreakoutColourLCD240x240_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_colourlcd240x240_globals, breakout_colourlcd240x240_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_colourlcd240x240_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_colourlcd240x240_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_colourlcd240x240, breakout_colourlcd240x240_user_cmodule, MODULE_BREAKOUT_COLOURLCD240X240_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////