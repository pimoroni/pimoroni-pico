#include "pico_unicorn.h"

/***** Constants *****/

enum buttons
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// picounicorn Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picounicorn__del__obj, picounicorn__del__);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picounicorn_set_pixel_obj, 6, 6, picounicorn_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picounicorn_set_pixel_value_obj, 4, 4, picounicorn_set_pixel_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picounicorn_clear_obj, picounicorn_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picounicorn_is_pressed_obj, picounicorn_is_pressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picounicorn_get_width_obj, picounicorn_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picounicorn_get_height_obj, picounicorn_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(picounicorn_update_obj, picounicorn_update);

STATIC const mp_rom_map_elem_t picounicorn_locals_dict_table[] = {   
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&picounicorn__del__obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picounicorn_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&picounicorn_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel_value), MP_ROM_PTR(&picounicorn_set_pixel_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picounicorn_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picounicorn_is_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picounicorn_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picounicorn_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};

STATIC MP_DEFINE_CONST_DICT(picounicorn_locals_dict, picounicorn_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    picounicorn_type,
    MP_QSTR_PicoUnicorn,
    MP_TYPE_FLAG_NONE,
    make_new, picounicorn_make_new,
    locals_dict, (mp_obj_dict_t*)&picounicorn_locals_dict
);
#else
const mp_obj_type_t picounicorn_type = {
    { &mp_type_type },
    .name = MP_QSTR_PicoUnicorn,
    .make_new = picounicorn_make_new,
    .locals_dict = (mp_obj_dict_t*)&picounicorn_locals_dict,
};
#endif

/***** Globals Table *****/
STATIC const mp_rom_map_elem_t picounicorn_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picounicorn) }, 
    { MP_OBJ_NEW_QSTR(MP_QSTR_PicoUnicorn), (mp_obj_t)&picounicorn_type },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(7) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picounicorn_globals, picounicorn_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picounicorn_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_picounicorn_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_picounicorn, picounicorn_user_cmodule, MODULE_PICOUNICORN_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_picounicorn, picounicorn_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////