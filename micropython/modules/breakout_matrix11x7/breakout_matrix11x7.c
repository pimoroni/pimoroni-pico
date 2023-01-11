#include "breakout_matrix11x7.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutMatrix11x7 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutMatrix11x7_set_pixel_obj, 1, BreakoutMatrix11x7_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMatrix11x7_update_obj, BreakoutMatrix11x7_update);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutMatrix11x7_clear_obj, BreakoutMatrix11x7_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutMatrix11x7_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&BreakoutMatrix11x7_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutMatrix11x7_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutMatrix11x7_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutMatrix11x7_locals_dict, BreakoutMatrix11x7_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_matrix11x7_BreakoutMatrix11x7_type,
    MP_QSTR_BreakoutMatrix11x7,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutMatrix11x7_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutMatrix11x7_locals_dict
);
#else
const mp_obj_type_t breakout_matrix11x7_BreakoutMatrix11x7_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutMatrix11x7,
    .make_new = BreakoutMatrix11x7_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutMatrix11x7_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_matrix11x7 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_matrix11x7_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_matrix11x7) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutMatrix11x7), (mp_obj_t)&breakout_matrix11x7_BreakoutMatrix11x7_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_matrix11x7_globals, breakout_matrix11x7_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_matrix11x7_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_matrix11x7_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_matrix11x7, breakout_matrix11x7_user_cmodule, MODULE_BREAKOUT_MATRIX11X7_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_matrix11x7, breakout_matrix11x7_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////