#include "breakout_colourlcd160x80.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutColourLCD160x80 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

// Module functions
STATIC MP_DEFINE_CONST_FUN_OBJ_3(BreakoutColourLCD160x80_module_RGB565_obj, BreakoutColourLCD160x80_module_RGB565);

// Class Methods
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD160x80_update_obj, BreakoutColourLCD160x80_update);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutColourLCD160x80_set_backlight_obj, BreakoutColourLCD160x80_set_backlight);

// Pen
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutColourLCD160x80_set_pen_obj, BreakoutColourLCD160x80_set_pen);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_create_pen_obj, 4, 4, BreakoutColourLCD160x80_create_pen);

// Primitives
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_set_clip_obj, 5, 5, BreakoutColourLCD160x80_set_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD160x80_remove_clip_obj, BreakoutColourLCD160x80_remove_clip);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD160x80_clear_obj, BreakoutColourLCD160x80_clear);
MP_DEFINE_CONST_FUN_OBJ_3(BreakoutColourLCD160x80_pixel_obj, BreakoutColourLCD160x80_pixel);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_pixel_span_obj, 4, 4, BreakoutColourLCD160x80_pixel_span);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_rectangle_obj, 5, 5, BreakoutColourLCD160x80_rectangle);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_circle_obj, 4, 4, BreakoutColourLCD160x80_circle);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD160x80_character_obj, 1, BreakoutColourLCD160x80_character);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD160x80_text_obj, 1, BreakoutColourLCD160x80_text);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD160x80_measure_text_obj, 1, BreakoutColourLCD160x80_measure_text);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutColourLCD160x80_polygon_obj, 2, BreakoutColourLCD160x80_polygon);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_triangle_obj, 7, 7, BreakoutColourLCD160x80_triangle);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(BreakoutColourLCD160x80_line_obj, 5, 5, BreakoutColourLCD160x80_line);

// Utility
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutColourLCD160x80_get_bounds_obj, BreakoutColourLCD160x80_get_bounds);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutColourLCD160x80_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&BreakoutColourLCD160x80_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&BreakoutColourLCD160x80_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutColourLCD160x80_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&BreakoutColourLCD160x80_set_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&BreakoutColourLCD160x80_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&BreakoutColourLCD160x80_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&BreakoutColourLCD160x80_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutColourLCD160x80_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&BreakoutColourLCD160x80_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&BreakoutColourLCD160x80_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&BreakoutColourLCD160x80_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&BreakoutColourLCD160x80_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&BreakoutColourLCD160x80_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure_text), MP_ROM_PTR(&BreakoutColourLCD160x80_measure_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon), MP_ROM_PTR(&BreakoutColourLCD160x80_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&BreakoutColourLCD160x80_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&BreakoutColourLCD160x80_line_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_bounds), MP_ROM_PTR(&BreakoutColourLCD160x80_get_bounds_obj) }
};
STATIC MP_DEFINE_CONST_DICT(BreakoutColourLCD160x80_locals_dict, BreakoutColourLCD160x80_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t BreakoutColourLCD160x80_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_colourlcd160x80,
    .make_new = BreakoutColourLCD160x80_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutColourLCD160x80_locals_dict,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_colourlcd160x80 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_colourlcd160x80_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_colourlcd160x80) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutColourLCD160x80), (mp_obj_t)&BreakoutColourLCD160x80_type },

    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_PTR(&BreakoutColourLCD160x80_module_RGB565_obj) },

    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_colourlcd160x80_globals, breakout_colourlcd160x80_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_colourlcd160x80_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_colourlcd160x80_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_colourlcd160x80, breakout_colourlcd160x80_user_cmodule, MODULE_BREAKOUT_COLOURLCD160X80_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////