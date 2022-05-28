#include "st7789.h"

// Module functions
STATIC MP_DEFINE_CONST_FUN_OBJ_3(GenericST7789_module_RGB332_obj, GenericST7789_module_RGB332);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(GenericST7789_module_RGB565_obj, GenericST7789_module_RGB565);

// Class Methods
MP_DEFINE_CONST_FUN_OBJ_1(GenericST7789_update_obj, GenericST7789_update);
MP_DEFINE_CONST_FUN_OBJ_2(GenericST7789_set_backlight_obj, GenericST7789_set_backlight);
MP_DEFINE_CONST_FUN_OBJ_2(GenericST7789_set_framebuffer_obj, GenericST7789_set_framebuffer);

// Palette management
MP_DEFINE_CONST_FUN_OBJ_2(GenericST7789_set_palette_mode_obj, GenericST7789_set_palette_mode);
MP_DEFINE_CONST_FUN_OBJ_3(GenericST7789_set_palette_obj, GenericST7789_set_palette);
MP_DEFINE_CONST_FUN_OBJ_1(GenericST7789_reserve_palette_obj, GenericST7789_reserve_palette);

// Pen
MP_DEFINE_CONST_FUN_OBJ_2(GenericST7789_set_pen_obj, GenericST7789_set_pen);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_create_pen_obj, 3, GenericST7789_create_pen);

// Primitives
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_set_clip_obj, 1, GenericST7789_set_clip);
MP_DEFINE_CONST_FUN_OBJ_1(GenericST7789_remove_clip_obj, GenericST7789_remove_clip);
MP_DEFINE_CONST_FUN_OBJ_1(GenericST7789_clear_obj, GenericST7789_clear);
MP_DEFINE_CONST_FUN_OBJ_3(GenericST7789_pixel_obj, GenericST7789_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_pixel_span_obj, 1, GenericST7789_pixel_span);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_rectangle_obj, 1, GenericST7789_rectangle);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_circle_obj, 1, GenericST7789_circle);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_character_obj, 1, GenericST7789_character);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_text_obj, 1, GenericST7789_text);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_measure_text_obj, 1, GenericST7789_measure_text);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_polygon_obj, 2, GenericST7789_polygon);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_triangle_obj, 1, GenericST7789_triangle);
MP_DEFINE_CONST_FUN_OBJ_KW(GenericST7789_line_obj, 1, GenericST7789_line);

// Utility
MP_DEFINE_CONST_FUN_OBJ_1(GenericST7789_get_bounds_obj, GenericST7789_get_bounds);

STATIC const mp_rom_map_elem_t GenericST7789_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&GenericST7789_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pen), MP_ROM_PTR(&GenericST7789_set_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&GenericST7789_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_palette_mode), MP_ROM_PTR(&GenericST7789_set_palette_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_palette), MP_ROM_PTR(&GenericST7789_set_palette_obj) },
    { MP_ROM_QSTR(MP_QSTR_reserve_palette), MP_ROM_PTR(&GenericST7789_reserve_palette_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&GenericST7789_set_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_pen), MP_ROM_PTR(&GenericST7789_create_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&GenericST7789_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_clip), MP_ROM_PTR(&GenericST7789_remove_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&GenericST7789_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_span), MP_ROM_PTR(&GenericST7789_pixel_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&GenericST7789_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&GenericST7789_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&GenericST7789_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&GenericST7789_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure_text), MP_ROM_PTR(&GenericST7789_measure_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon), MP_ROM_PTR(&GenericST7789_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&GenericST7789_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&GenericST7789_line_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_bounds), MP_ROM_PTR(&GenericST7789_get_bounds_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_framebuffer), MP_ROM_PTR(&GenericST7789_set_framebuffer_obj) },
};
STATIC MP_DEFINE_CONST_DICT(GenericST7789_locals_dict, GenericST7789_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t GenericST7789_type = {
    { &mp_type_type },
    .name = MP_QSTR_st7789,
    .make_new = GenericST7789_make_new,
    .locals_dict = (mp_obj_dict_t*)&GenericST7789_locals_dict,
};

/***** Module Globals *****/
STATIC const mp_map_elem_t st7789_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_st7789) },
    { MP_ROM_QSTR(MP_QSTR_ST7789), (mp_obj_t)&GenericST7789_type },

    { MP_ROM_QSTR(MP_QSTR_RGB332), MP_ROM_PTR(&GenericST7789_module_RGB332_obj) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_PTR(&GenericST7789_module_RGB565_obj) },

    { MP_ROM_QSTR(MP_QSTR_PALETTE_RGB332), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PALETTE_USER), MP_ROM_INT(1) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY_LCD_240X240), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_ROUND_LCD_240X240), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_PICO_DISPLAY), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_PICO_DISPLAY_2), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_PICO_EXPLORER), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_TUFTY_2040), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_ENVIRO_PLUS), MP_ROM_INT(6) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_st7789_globals, st7789_globals_table);

/***** Module Definition *****/
const mp_obj_module_t st7789_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_st7789_globals,
};

MP_REGISTER_MODULE(MP_QSTR_st7789, st7789_user_cmodule, MODULE_ST7789_ENABLED);