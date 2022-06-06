#include "inky2040.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Inky2040___del___obj, Inky2040___del__);

MP_DEFINE_CONST_FUN_OBJ_1(Inky2040_is_busy_obj, Inky2040_is_busy);
MP_DEFINE_CONST_FUN_OBJ_1(Inky2040_update_obj, Inky2040_update);

MP_DEFINE_CONST_FUN_OBJ_2(Inky2040_font_obj, Inky2040_font);
MP_DEFINE_CONST_FUN_OBJ_2(Inky2040_pen_obj, Inky2040_pen);
MP_DEFINE_CONST_FUN_OBJ_2(Inky2040_thickness_obj, Inky2040_thickness);

MP_DEFINE_CONST_FUN_OBJ_1(Inky2040_clear_obj, Inky2040_clear);
MP_DEFINE_CONST_FUN_OBJ_3(Inky2040_pixel_obj, Inky2040_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_line_obj, 4, Inky2040_line);
MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_rectangle_obj, 4, Inky2040_rectangle);

MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_icon_obj, 4, Inky2040_icon);
MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_image_obj, 2, Inky2040_image);

MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_text_obj, 4, Inky2040_text);
MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_glyph_obj, 4, Inky2040_glyph);

MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_measure_text_obj, 2, Inky2040_measure_text);
MP_DEFINE_CONST_FUN_OBJ_KW(Inky2040_measure_glyph_obj, 2, Inky2040_measure_glyph);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Inky2040_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Inky2040___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_is_busy), MP_ROM_PTR(&Inky2040_is_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&Inky2040_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_font), MP_ROM_PTR(&Inky2040_font_obj) },
    { MP_ROM_QSTR(MP_QSTR_pen), MP_ROM_PTR(&Inky2040_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_thickness), MP_ROM_PTR(&Inky2040_thickness_obj) },

    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&Inky2040_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&Inky2040_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&Inky2040_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&Inky2040_rectangle_obj) },

    { MP_ROM_QSTR(MP_QSTR_icon), MP_ROM_PTR(&Inky2040_icon_obj) },
    { MP_ROM_QSTR(MP_QSTR_image), MP_ROM_PTR(&Inky2040_image_obj) },

    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&Inky2040_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_glyph), MP_ROM_PTR(&Inky2040_glyph_obj) },

    { MP_ROM_QSTR(MP_QSTR_measure_text), MP_ROM_PTR(&Inky2040_measure_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure_glyph), MP_ROM_PTR(&Inky2040_measure_glyph_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Inky2040_locals_dict, Inky2040_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t Inky2040_type = {
    { &mp_type_type },
    .name = MP_QSTR_inky2040,
    .print = Inky2040_print,
    .make_new = Inky2040_make_new,
    .locals_dict = (mp_obj_dict_t*)&Inky2040_locals_dict,
};

/***** Globals Table *****/

STATIC const mp_rom_map_elem_t inky2040_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_inky2040) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Inky2040), (mp_obj_t)&Inky2040_type },

    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(600) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(448) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_C), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_D), MP_ROM_INT(15) },

    { MP_ROM_QSTR(MP_QSTR_PIN_CS), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_PIN_CLK), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOSI), MP_ROM_INT(19) },
    { MP_ROM_QSTR(MP_QSTR_PIN_DC), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_PIN_RESET), MP_ROM_INT(25) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BUSY), MP_ROM_INT(26) },

    { MP_ROM_QSTR(MP_QSTR_PIN_VBUS_DETECT), MP_ROM_INT(24) },
    { MP_ROM_QSTR(MP_QSTR_PIN_VREF_POWER), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_PIN_1V2_REF), MP_ROM_INT(28) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BATTERY), MP_ROM_INT(29) },
    { MP_ROM_QSTR(MP_QSTR_PIN_ENABLE_3V3), MP_ROM_INT(10) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_inky2040_globals, inky2040_globals_table);

/***** Module Definition *****/
const mp_obj_module_t inky2040_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_inky2040_globals,
};
MP_REGISTER_MODULE(MP_QSTR_inky2040, inky2040_user_cmodule, MODULE_INKY2040_ENABLED);
