#include "breakout_trackball.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutTrackball Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_change_address_obj, 1, BreakoutTrackball_change_address);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_enable_interrupt_obj, 1, BreakoutTrackball_change_address);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTrackball_get_interrupt_obj, BreakoutTrackball_get_interrupt);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_set_rgbw_obj, 1, BreakoutTrackball_set_rgbw);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_set_red_obj, 1, BreakoutTrackball_set_red);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_set_green_obj, 1, BreakoutTrackball_set_green);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_set_blue_obj, 1, BreakoutTrackball_set_blue);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutTrackball_set_white_obj, 1, BreakoutTrackball_set_white);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutTrackball_read_obj, BreakoutTrackball_read);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutTrackball_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_change_address), MP_ROM_PTR(&BreakoutTrackball_change_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupt), MP_ROM_PTR(&BreakoutTrackball_enable_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_interrupt), MP_ROM_PTR(&BreakoutTrackball_get_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgbw), MP_ROM_PTR(&BreakoutTrackball_set_rgbw_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_red), MP_ROM_PTR(&BreakoutTrackball_set_red_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_green), MP_ROM_PTR(&BreakoutTrackball_set_green_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_blue), MP_ROM_PTR(&BreakoutTrackball_set_blue_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_white), MP_ROM_PTR(&BreakoutTrackball_set_white_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutTrackball_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_INT(LEFT) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_INT(RIGHT) },
    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(UP) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(DOWN) },
    { MP_ROM_QSTR(MP_QSTR_SW_CHANGED), MP_ROM_INT(SW_CHANGED) },
    { MP_ROM_QSTR(MP_QSTR_SW_PRESSED), MP_ROM_INT(SW_PRESSED) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutTrackball_locals_dict, BreakoutTrackball_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_trackball_BreakoutTrackball_type,
    MP_QSTR_BreakoutTrackball,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutTrackball_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutTrackball_locals_dict
);
#else
const mp_obj_type_t breakout_trackball_BreakoutTrackball_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutTrackball,
    .make_new = BreakoutTrackball_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutTrackball_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_trackball Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_trackball_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_trackball) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutTrackball), (mp_obj_t)&breakout_trackball_BreakoutTrackball_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_trackball_globals, breakout_trackball_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_trackball_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_trackball_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_trackball, breakout_trackball_user_cmodule, MODULE_BREAKOUT_TRACKBALL_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_trackball, breakout_trackball_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////