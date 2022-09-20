#include "breakout_rgbmatrix5x5.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutRGBMatrix5x5 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRGBMatrix5x5_set_pixel_obj, 1, BreakoutRGBMatrix5x5_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRGBMatrix5x5_update_obj, BreakoutRGBMatrix5x5_update);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRGBMatrix5x5_clear_obj, BreakoutRGBMatrix5x5_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutRGBMatrix5x5_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&BreakoutRGBMatrix5x5_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&BreakoutRGBMatrix5x5_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&BreakoutRGBMatrix5x5_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(HEIGHT) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutRGBMatrix5x5_locals_dict, BreakoutRGBMatrix5x5_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_rgbmatrix5x5_BreakoutRGBMatrix5x5_type,
    MP_QSTR_BreakoutRGBMatrix5x5,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutRGBMatrix5x5_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutRGBMatrix5x5_locals_dict
);
#else
const mp_obj_type_t breakout_rgbmatrix5x5_BreakoutRGBMatrix5x5_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutRGBMatrix5x5,
    .make_new = BreakoutRGBMatrix5x5_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutRGBMatrix5x5_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_rgbmatrix5x5 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_rgbmatrix5x5_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_rgbmatrix5x5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutRGBMatrix5x5), (mp_obj_t)&breakout_rgbmatrix5x5_BreakoutRGBMatrix5x5_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_rgbmatrix5x5_globals, breakout_rgbmatrix5x5_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_rgbmatrix5x5_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_rgbmatrix5x5_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_rgbmatrix5x5, breakout_rgbmatrix5x5_user_cmodule, MODULE_BREAKOUT_RGBMATRIX5X5_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_rgbmatrix5x5, breakout_rgbmatrix5x5_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////