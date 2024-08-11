#include "badger2040.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040___del___obj, Badger2040___del__);

MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_update_speed_obj, Badger2040_update_speed);
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_is_busy_obj, Badger2040_is_busy);
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_update_obj, Badger2040_update);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_partial_update_obj, 4, Badger2040_partial_update);

MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_invert_obj, Badger2040_invert);
MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_led_obj, Badger2040_led);
MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_font_obj, Badger2040_font);
MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_pen_obj, Badger2040_pen);
MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_thickness_obj, Badger2040_thickness);

MP_DEFINE_CONST_FUN_OBJ_2(Badger2040_pressed_obj, Badger2040_pressed);
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_clear_obj, Badger2040_clear);
MP_DEFINE_CONST_FUN_OBJ_3(Badger2040_pixel_obj, Badger2040_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_line_obj, 4, Badger2040_line);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_rectangle_obj, 4, Badger2040_rectangle);

MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_icon_obj, 4, Badger2040_icon);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_image_obj, 2, Badger2040_image);

MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_text_obj, 4, Badger2040_text);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_glyph_obj, 4, Badger2040_glyph);

MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_measure_text_obj, 2, Badger2040_measure_text);
MP_DEFINE_CONST_FUN_OBJ_KW(Badger2040_measure_glyph_obj, 2, Badger2040_measure_glyph);

MP_DEFINE_CONST_FUN_OBJ_3(Badger2040_command_obj, Badger2040_command);

MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_pressed_to_wake_obj, Badger2040_pressed_to_wake);
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_halt_obj, Badger2040_halt);
MP_DEFINE_CONST_FUN_OBJ_0(Badger2040_woken_by_button_obj, Badger2040_woken_by_button);
MP_DEFINE_CONST_FUN_OBJ_1(Badger2040_system_speed_obj, Badger2040_system_speed);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Badger2040_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Badger2040___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_is_busy), MP_ROM_PTR(&Badger2040_is_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_speed), MP_ROM_PTR(&Badger2040_update_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&Badger2040_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_partial_update), MP_ROM_PTR(&Badger2040_partial_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_halt), MP_ROM_PTR(&Badger2040_halt_obj) },

    { MP_ROM_QSTR(MP_QSTR_invert), MP_ROM_PTR(&Badger2040_invert_obj) },
    { MP_ROM_QSTR(MP_QSTR_led), MP_ROM_PTR(&Badger2040_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_font), MP_ROM_PTR(&Badger2040_font_obj) },
    { MP_ROM_QSTR(MP_QSTR_pen), MP_ROM_PTR(&Badger2040_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_thickness), MP_ROM_PTR(&Badger2040_thickness_obj) },

    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&Badger2040_pressed_obj) },

    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&Badger2040_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&Badger2040_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&Badger2040_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&Badger2040_rectangle_obj) },

    { MP_ROM_QSTR(MP_QSTR_icon), MP_ROM_PTR(&Badger2040_icon_obj) },
    { MP_ROM_QSTR(MP_QSTR_image), MP_ROM_PTR(&Badger2040_image_obj) },

    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&Badger2040_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_glyph), MP_ROM_PTR(&Badger2040_glyph_obj) },

    { MP_ROM_QSTR(MP_QSTR_measure_text), MP_ROM_PTR(&Badger2040_measure_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_measure_glyph), MP_ROM_PTR(&Badger2040_measure_glyph_obj) },

    { MP_ROM_QSTR(MP_QSTR_command), MP_ROM_PTR(&Badger2040_command_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Badger2040_locals_dict, Badger2040_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    Badger2040_type,
    MP_QSTR_Badger2040,
    MP_TYPE_FLAG_NONE,
    make_new, Badger2040_make_new,
    locals_dict, (mp_obj_dict_t*)&Badger2040_locals_dict
);
#else
const mp_obj_type_t Badger2040_type = {
    { &mp_type_type },
    .name = MP_QSTR_Badger2040,
    .make_new = Badger2040_make_new,
    .locals_dict = (mp_obj_dict_t*)&Badger2040_locals_dict,
};
#endif

/***** Globals Table *****/

STATIC const mp_rom_map_elem_t badger2040_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_badger2040) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Badger2040), (mp_obj_t)&Badger2040_type },

    { MP_ROM_QSTR(MP_QSTR_pressed_to_wake), MP_ROM_PTR(&Badger2040_pressed_to_wake_obj) },
    { MP_ROM_QSTR(MP_QSTR_woken_by_button), MP_ROM_PTR(&Badger2040_woken_by_button_obj) },
    { MP_ROM_QSTR(MP_QSTR_system_speed), MP_ROM_PTR(&Badger2040_system_speed_obj) },

    { MP_ROM_QSTR(MP_QSTR_UPDATE_NORMAL), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_UPDATE_MEDIUM), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_UPDATE_FAST), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_UPDATE_TURBO), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_UPDATE_SUPER_EXTRA_TURBO), MP_ROM_INT(3) }, // ho ho placebo!

    { MP_ROM_QSTR(MP_QSTR_SYSTEM_VERY_SLOW), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SYSTEM_SLOW), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SYSTEM_NORMAL), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_SYSTEM_FAST), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_SYSTEM_TURBO), MP_ROM_INT(4) },

    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(296) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(128) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(13) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_C), MP_ROM_INT(14) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_D), MP_ROM_INT(15) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_E), MP_ROM_INT(11) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_INT(15) },    // alias for D
    { MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_INT(11) },  // alias for E
    { MP_ROM_QSTR(MP_QSTR_BUTTON_USER), MP_ROM_INT(23) },

    { MP_ROM_QSTR(MP_QSTR_PIN_CS), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_PIN_CLK), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_PIN_MOSI), MP_ROM_INT(19) },
    { MP_ROM_QSTR(MP_QSTR_PIN_DC), MP_ROM_INT(20) },
    { MP_ROM_QSTR(MP_QSTR_PIN_RESET), MP_ROM_INT(21) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BUSY), MP_ROM_INT(26) },

    { MP_ROM_QSTR(MP_QSTR_PIN_VBUS_DETECT), MP_ROM_INT(24) },
    { MP_ROM_QSTR(MP_QSTR_PIN_LED), MP_ROM_INT(25) },
    { MP_ROM_QSTR(MP_QSTR_PIN_VREF_POWER), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_PIN_1V2_REF), MP_ROM_INT(28) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BATTERY), MP_ROM_INT(29) },
    { MP_ROM_QSTR(MP_QSTR_PIN_ENABLE_3V3), MP_ROM_INT(10) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_badger2040_globals, badger2040_globals_table);

/***** Module Definition *****/
const mp_obj_module_t badger2040_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_badger2040_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_badger2040, badger2040_user_cmodule, MODULE_BADGER2040_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_badger2040, badger2040_user_cmodule);
#endif
