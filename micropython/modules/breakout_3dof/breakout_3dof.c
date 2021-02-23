#include "breakout_3dof.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Breakout3DOF Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(Breakout3DOF_get_axis_obj, 1, Breakout3DOF_get_axis);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Breakout3DOF_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_axis), MP_ROM_PTR(&Breakout3DOF_get_axis_obj) },
    { MP_ROM_QSTR(MP_QSTR_X), MP_ROM_INT(AXIS_X) },
    { MP_ROM_QSTR(MP_QSTR_Y), MP_ROM_INT(AXIS_Y) },
    { MP_ROM_QSTR(MP_QSTR_Z), MP_ROM_INT(AXIS_Z) },
};
STATIC MP_DEFINE_CONST_DICT(Breakout3DOF_locals_dict, Breakout3DOF_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_3dof_Breakout3DOF_type = {
    { &mp_type_type },
    .name = MP_QSTR_breakout_3dof,
    .print = Breakout3DOF_print,
    .make_new = Breakout3DOF_make_new,
    .locals_dict = (mp_obj_dict_t*)&Breakout3DOF_locals_dict,
};



////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_3dof Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_3dof_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_3dof) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Breakout3DOF), (mp_obj_t)&breakout_3dof_Breakout3DOF_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_3dof_globals, breakout_3dof_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_3dof_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_3dof_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_breakout_3dof, breakout_3dof_user_cmodule, MODULE_BREAKOUT_3DOF_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////