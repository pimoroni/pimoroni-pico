#include "breakout_ltr559.h"

/***** Constants *****/
// enum buttons
// {
//     BUTTON_A = 0,
//     BUTTON_B,
//     BUTTON_X,
//     BUTTON_Y,
// };


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_ltr559 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(breakout_ltr559_init_obj, breakout_ltr559_init);
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(breakout_ltr559_get_width_obj, breakout_ltr559_get_width);
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(breakout_ltr559_get_height_obj, breakout_ltr559_get_height);
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(breakout_ltr559_update_obj, breakout_ltr559_update);
// STATIC MP_DEFINE_CONST_FUN_OBJ_3(breakout_ltr559_set_pixel_obj, breakout_ltr559_set_pixel);
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(breakout_ltr559_clear_obj, breakout_ltr559_clear);
// STATIC MP_DEFINE_CONST_FUN_OBJ_1(breakout_ltr559_is_pressed_obj, breakout_ltr559_is_pressed);

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_ltr559_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_breakout_ltr559) },
//     { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&breakout_ltr559_init_obj) },    
//     { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&breakout_ltr559_get_width_obj) },
//     { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&breakout_ltr559_get_height_obj) },
//     { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&breakout_ltr559_update_obj) },
//     { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&breakout_ltr559_set_pixel_obj) },
//     { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&breakout_ltr559_clear_obj) },
//     { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&breakout_ltr559_is_pressed_obj) },        
//     { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_INT(BUTTON_A) },
//     { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_INT(BUTTON_B) },
//     { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_INT(BUTTON_X) },
//     { MP_ROM_QSTR(MP_QSTR_BUTTON_Y), MP_ROM_INT(BUTTON_Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_ltr559_globals, breakout_ltr559_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_ltr559_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_ltr559_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_ltr559, breakout_ltr559_user_cmodule, MODULE_BREAKOUT_LTR559_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////