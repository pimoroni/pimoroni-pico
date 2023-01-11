#include "breakout_pmw3901.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutPMW3901 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPMW3901_get_id_obj, BreakoutPMW3901_get_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutPMW3901_get_revision_obj, BreakoutPMW3901_get_revision);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPMW3901_set_rotation_obj, 1, BreakoutPMW3901_set_rotation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPMW3901_set_orientation_obj, 1, BreakoutPMW3901_set_orientation);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPMW3901_get_motion_obj, 1, BreakoutPMW3901_get_motion);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPMW3901_get_motion_slow_obj, 1, BreakoutPMW3901_get_motion_slow);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutPMW3901_frame_capture_obj, 2, BreakoutPMW3901_frame_capture);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutPMW3901_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_id), MP_ROM_PTR(&BreakoutPMW3901_get_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_revision), MP_ROM_PTR(&BreakoutPMW3901_get_revision_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rotation), MP_ROM_PTR(&BreakoutPMW3901_set_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_orientation), MP_ROM_PTR(&BreakoutPMW3901_set_orientation_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_motion), MP_ROM_PTR(&BreakoutPMW3901_get_motion_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_motion_slow), MP_ROM_PTR(&BreakoutPMW3901_get_motion_slow_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_capture), MP_ROM_PTR(&BreakoutPMW3901_frame_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_0), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_90), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_180), MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_DEGREES_270), MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_FRAME_SIZE), MP_ROM_INT(35) },
    { MP_ROM_QSTR(MP_QSTR_FRAME_BYTES), MP_ROM_INT(1225) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutPMW3901_locals_dict, BreakoutPMW3901_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_pmw3901_BreakoutPMW3901_type,
    MP_QSTR_BreakoutPMW3901,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutPMW3901_make_new,
    print, BreakoutPMW3901_print,
    locals_dict, (mp_obj_dict_t*)&BreakoutPMW3901_locals_dict
);
#else
const mp_obj_type_t breakout_pmw3901_BreakoutPMW3901_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutPMW3901,
    .print = BreakoutPMW3901_print,
    .make_new = BreakoutPMW3901_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutPMW3901_locals_dict,
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_pmw3901 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_pmw3901_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_pmw3901) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutPMW3901), (mp_obj_t)&breakout_pmw3901_BreakoutPMW3901_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_pmw3901_globals, breakout_pmw3901_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_pmw3901_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_pmw3901_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_pmw3901, breakout_pmw3901_user_cmodule, MODULE_BREAKOUT_PMW3901_ENABLED);//,
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_pmw3901, breakout_pmw3901_user_cmodule);
#endif