#include "breakout_roundlcd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutRoundLCD Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRoundLCD_update_obj, BreakoutRoundLCD_update);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_set_backlight_obj, 1, BreakoutRoundLCD_set_backlight);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_set_pen_obj, 1, BreakoutRoundLCD_set_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_create_pen_obj, 1, BreakoutRoundLCD_create_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_set_clip_obj, 1, BreakoutRoundLCD_set_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRoundLCD_remove_clip_obj, BreakoutRoundLCD_remove_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRoundLCD_clear_obj, BreakoutRoundLCD_clear);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_pixel_obj, 1, BreakoutRoundLCD_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_pixel_span_obj, 1, BreakoutRoundLCD_pixel_span);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_rectangle_obj, 1, BreakoutRoundLCD_rectangle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_circle_obj, 1, BreakoutRoundLCD_circle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_character_obj, 1, BreakoutRoundLCD_character);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_text_obj, 1, BreakoutRoundLCD_text);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_polygon_obj, 1, BreakoutRoundLCD_polygon);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_triangle_obj, 1, BreakoutRoundLCD_triangle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRoundLCD_line_obj, 1, BreakoutRoundLCD_line);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutRoundLCD_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutRoundLCD_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&BreakoutRoundLCD_set_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&BreakoutRoundLCD_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&BreakoutRoundLCD_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&BreakoutRoundLCD_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&BreakoutRoundLCD_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutRoundLCD_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&BreakoutRoundLCD_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&BreakoutRoundLCD_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&BreakoutRoundLCD_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&BreakoutRoundLCD_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&BreakoutRoundLCD_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&BreakoutRoundLCD_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon), MP_ROM_PTR(&BreakoutRoundLCD_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&BreakoutRoundLCD_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&BreakoutRoundLCD_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutRoundLCD_locals_dict, BreakoutRoundLCD_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_roundlcd_BreakoutRoundLCD_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_roundlcd,
    .print = BreakoutRoundLCD_print,
    .make_new = BreakoutRoundLCD_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutRoundLCD_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_roundlcd Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_roundlcd_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_roundlcd) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutRoundLCD), (mp_obj_t)&breakout_roundlcd_BreakoutRoundLCD_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_roundlcd_globals, breakout_roundlcd_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_roundlcd_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_roundlcd_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_roundlcd, breakout_roundlcd_user_cmodule, MODULE_BREAKOUT_ROUNDLCD_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////