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
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picounicorn_init_obj, picounicorn_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picounicorn_get_width_obj, picounicorn_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picounicorn_get_height_obj, picounicorn_get_height);
//STATIC MP_DEFINE_CONST_FUN_OBJ_0(picounicorn_update_obj, picounicorn_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picounicorn_set_pixel_obj, 5, 5, picounicorn_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picounicorn_set_pixel_value_obj, picounicorn_set_pixel_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picounicorn_clear_obj, picounicorn_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picounicorn_is_pressed_obj, picounicorn_is_pressed);

/***** Globals Table *****/
STATIC const mp_rom_map_elem_t picounicorn_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picounicorn) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picounicorn_init_obj) },    
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picounicorn_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picounicorn_get_height_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picounicorn_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&picounicorn_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel_value), MP_ROM_PTR(&picounicorn_set_pixel_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picounicorn_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picounicorn_is_pressed_obj) },        
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
MP_REGISTER_MODULE(MP_QSTR_picounicorn, picounicorn_user_cmodule, MODULE_PICOUNICORN_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////