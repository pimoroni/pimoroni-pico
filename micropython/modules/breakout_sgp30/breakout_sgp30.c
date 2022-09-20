#include "breakout_sgp30.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutSGP30 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_retrieve_unique_id_obj, BreakoutSGP30_retrieve_unique_id);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_get_unique_id_obj, BreakoutSGP30_get_unique_id);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutSGP30_start_measurement_obj, 2, BreakoutSGP30_start_measurement);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_get_air_quality_obj, BreakoutSGP30_get_air_quality);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_get_air_quality_raw_obj, BreakoutSGP30_get_air_quality_raw);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_soft_reset_obj, BreakoutSGP30_soft_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutSGP30_get_baseline_obj, BreakoutSGP30_get_baseline);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutSGP30_set_baseline_obj, 3,BreakoutSGP30_set_baseline);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutSGP30_set_humidity_obj, 2, BreakoutSGP30_set_humidity);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutSGP30_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_retrieve_unique_id), MP_ROM_PTR(&BreakoutSGP30_retrieve_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_unique_id), MP_ROM_PTR(&BreakoutSGP30_get_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_measurement), MP_ROM_PTR(&BreakoutSGP30_start_measurement_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_air_quality), MP_ROM_PTR(&BreakoutSGP30_get_air_quality_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_air_quality_raw), MP_ROM_PTR(&BreakoutSGP30_get_air_quality_raw_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&BreakoutSGP30_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_baseline), MP_ROM_PTR(&BreakoutSGP30_get_baseline_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_baseline), MP_ROM_PTR(&BreakoutSGP30_set_baseline_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_humidity), MP_ROM_PTR(&BreakoutSGP30_set_humidity_obj) },
    { MP_ROM_QSTR(MP_QSTR_ECO2), MP_ROM_INT(ECO2) },
    { MP_ROM_QSTR(MP_QSTR_TVOC), MP_ROM_INT(TVOC) },
    { MP_ROM_QSTR(MP_QSTR_H2), MP_ROM_INT(H2) },
    { MP_ROM_QSTR(MP_QSTR_ETHANOL), MP_ROM_INT(ETHANOL) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutSGP30_locals_dict, BreakoutSGP30_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_sgp30_BreakoutSGP30_type,
    MP_QSTR_BreakoutSGP30,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutSGP30_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutSGP30_locals_dict
);
#else
const mp_obj_type_t breakout_sgp30_BreakoutSGP30_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutSGP30,
    .make_new = BreakoutSGP30_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutSGP30_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_sgp30 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_sgp30_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_breakout_sgp30) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutSGP30), (mp_obj_t)&breakout_sgp30_BreakoutSGP30_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_sgp30_globals, breakout_sgp30_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_sgp30_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_sgp30_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_sgp30, breakout_sgp30_user_cmodule, MODULE_BREAKOUT_SGP30_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_sgp30, breakout_sgp30_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////