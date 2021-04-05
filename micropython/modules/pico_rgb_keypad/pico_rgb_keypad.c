#include "pico_rgb_keypad.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// picokeypad Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_init_obj, picokeypad_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_width_obj, picokeypad_get_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_height_obj, picokeypad_get_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_num_pads_obj, picokeypad_get_num_pads);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_update_obj, picokeypad_update);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picokeypad_set_brightness_obj, picokeypad_set_brightness);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picokeypad_illuminate_xy_obj, 5, 5, picokeypad_illuminate_xy);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picokeypad_illuminate_obj, 4, 4, picokeypad_illuminate);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_clear_obj, picokeypad_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picokeypad_get_button_states_obj, picokeypad_get_button_states);

/***** Globals Table *****/
STATIC const mp_map_elem_t picokeypad_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picokeypad) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picokeypad_init_obj) },    
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&picokeypad_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&picokeypad_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_num_pads), MP_ROM_PTR(&picokeypad_get_num_pads_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&picokeypad_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&picokeypad_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_illuminate_xy), MP_ROM_PTR(&picokeypad_illuminate_xy_obj) },
    { MP_ROM_QSTR(MP_QSTR_illuminate), MP_ROM_PTR(&picokeypad_illuminate_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picokeypad_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_button_states), MP_ROM_PTR(&picokeypad_get_button_states_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picokeypad_globals, picokeypad_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picokeypad_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picokeypad_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picokeypad, picokeypad_user_cmodule, MODULE_PICOKEYPAD_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
