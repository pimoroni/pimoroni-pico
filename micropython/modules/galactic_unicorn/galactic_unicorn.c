#include "galactic_unicorn.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(GalacticUnicorn___del___obj, GalacticUnicorn___del__);
MP_DEFINE_CONST_FUN_OBJ_1(GalacticUnicorn_clear_obj, GalacticUnicorn_clear);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_update_obj, GalacticUnicorn_update);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_set_brightness_obj, GalacticUnicorn_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_1(GalacticUnicorn_get_brightness_obj, GalacticUnicorn_get_brightness);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_adjust_brightness_obj, GalacticUnicorn_adjust_brightness);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_set_volume_obj, GalacticUnicorn_set_volume);
MP_DEFINE_CONST_FUN_OBJ_1(GalacticUnicorn_get_volume_obj, GalacticUnicorn_get_volume);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_adjust_volume_obj, GalacticUnicorn_adjust_volume);
MP_DEFINE_CONST_FUN_OBJ_KW(GalacticUnicorn_set_pixel_obj, 4, GalacticUnicorn_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_1(GalacticUnicorn_light_obj, GalacticUnicorn_light);
MP_DEFINE_CONST_FUN_OBJ_2(GalacticUnicorn_is_pressed_obj, GalacticUnicorn_is_pressed);
//MP_DEFINE_CONST_FUN_OBJ_KW(GalacticUnicorn_play_sample_obj, 1, GalacticUnicorn_play_sample);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t GalacticUnicorn_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&GalacticUnicorn___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&GalacticUnicorn_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&GalacticUnicorn_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&GalacticUnicorn_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_brightness), MP_ROM_PTR(&GalacticUnicorn_get_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_adjust_brightness), MP_ROM_PTR(&GalacticUnicorn_adjust_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_volume), MP_ROM_PTR(&GalacticUnicorn_set_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_volume), MP_ROM_PTR(&GalacticUnicorn_get_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_adjust_volume), MP_ROM_PTR(&GalacticUnicorn_adjust_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&GalacticUnicorn_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_light), MP_ROM_PTR(&GalacticUnicorn_light_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&GalacticUnicorn_is_pressed_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_play_sample), MP_ROM_PTR(&GalacticUnicorn_play_sample_obj) },

    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(53) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(11) },

    { MP_ROM_QSTR(MP_QSTR_SWITCH_A), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_B), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_C), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_D), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_SLEEP), MP_ROM_INT(27) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_VOLUME_UP), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_VOLUME_DOWN), MP_ROM_INT(8) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_BRIGHTNESS_UP), MP_ROM_INT(21) },
    { MP_ROM_QSTR(MP_QSTR_SWITCH_BRIGHTNESS_DOWN), MP_ROM_INT(26) },
};

STATIC MP_DEFINE_CONST_DICT(GalacticUnicorn_locals_dict, GalacticUnicorn_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t GalacticUnicorn_type = {
    { &mp_type_type },
    .name = MP_QSTR_GalacticUnicorn,
    .print = GalacticUnicorn_print,
    .make_new = GalacticUnicorn_make_new,
    .locals_dict = (mp_obj_dict_t*)&GalacticUnicorn_locals_dict,
};

/***** Globals Table *****/
STATIC const mp_map_elem_t galactic_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_galactic) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GalacticUnicorn), (mp_obj_t)&GalacticUnicorn_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_galactic_globals, galactic_globals_table);

/***** Module Definition *****/
const mp_obj_module_t galactic_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_galactic_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_galactic, galactic_user_cmodule, MODULE_GALACTIC_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_galactic, galactic_user_cmodule);
#endif
