#include "breakout_dotmatrix.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutDotMatrix Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutDotMatrix_set_brightness_obj, 2, BreakoutDotMatrix_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutDotMatrix_set_decimal_obj, 1, BreakoutDotMatrix_set_decimal);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutDotMatrix_set_pixel_obj, 4, BreakoutDotMatrix_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutDotMatrix_set_character_obj, 3, BreakoutDotMatrix_set_character);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutDotMatrix_set_image_obj, 4, BreakoutDotMatrix_set_image);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutDotMatrix_clear_obj, BreakoutDotMatrix_clear);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutDotMatrix_show_obj, BreakoutDotMatrix_show);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutDotMatrix_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&BreakoutDotMatrix_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_decimal), MP_ROM_PTR(&BreakoutDotMatrix_set_decimal_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&BreakoutDotMatrix_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_character), MP_ROM_PTR(&BreakoutDotMatrix_set_character_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_image), MP_ROM_PTR(&BreakoutDotMatrix_set_image_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutDotMatrix_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&BreakoutDotMatrix_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutDotMatrix_locals_dict, BreakoutDotMatrix_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_dotmatrix_BreakoutDotMatrix_type,
    MP_QSTR_BreakoutDotMatrix,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutDotMatrix_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutDotMatrix_locals_dict
);
#else
const mp_obj_type_t breakout_dotmatrix_BreakoutDotMatrix_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutDotMatrix,
    .make_new = BreakoutDotMatrix_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutDotMatrix_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_dotmatrix Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_dotmatrix_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_dotmatrix) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutDotMatrix), (mp_obj_t)&breakout_dotmatrix_BreakoutDotMatrix_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_dotmatrix_globals, breakout_dotmatrix_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_dotmatrix_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_dotmatrix_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_dotmatrix, breakout_dotmatrix_user_cmodule, MODULE_BREAKOUT_DOTMATRIX_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_dotmatrix, breakout_dotmatrix_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////