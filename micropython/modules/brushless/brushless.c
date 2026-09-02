#include "brushless.h"

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Brushless___del___obj, Brushless___del__);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_pins_obj, Brushless_pins);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_has_inverses_obj, Brushless_has_inverses);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_enable_obj, Brushless_enable);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_disable_obj, Brushless_disable);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_is_enabled_obj, Brushless_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_u_duty_obj, Brushless_u_duty);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_v_duty_obj, Brushless_v_duty);
MP_DEFINE_CONST_FUN_OBJ_1(Brushless_w_duty_obj, Brushless_w_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(Brushless_duties_obj, 4, Brushless_duties);
MP_DEFINE_CONST_FUN_OBJ_KW(Brushless_frequency_obj, 1, Brushless_frequency);
MP_DEFINE_CONST_FUN_OBJ_KW(Brushless_direction_obj, 1, Brushless_direction);

MP_DEFINE_CONST_FUN_OBJ_1(BrushlessCluster___del___obj, BrushlessCluster___del__);
MP_DEFINE_CONST_FUN_OBJ_1(BrushlessCluster_count_obj, BrushlessCluster_count);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_pins_obj, 2, BrushlessCluster_pins);
MP_DEFINE_CONST_FUN_OBJ_1(BrushlessCluster_has_inverses_obj, BrushlessCluster_has_inverses);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_enable_obj, 2, BrushlessCluster_enable);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_enable_all_obj, 1, BrushlessCluster_enable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_disable_obj, 2, BrushlessCluster_disable);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_disable_all_obj, 1, BrushlessCluster_disable_all);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_is_enabled_obj, 2, BrushlessCluster_is_enabled);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_u_duty_obj, 2, BrushlessCluster_u_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_v_duty_obj, 2, BrushlessCluster_v_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_w_duty_obj, 2, BrushlessCluster_w_duty);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_duties_obj, 5, BrushlessCluster_duties);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_all_to_duties_obj, 4, BrushlessCluster_all_to_duties);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_phase_obj, 2, BrushlessCluster_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_all_to_phase_obj, 2, BrushlessCluster_all_to_phase);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_frequency_obj, 1, BrushlessCluster_frequency);
MP_DEFINE_CONST_FUN_OBJ_KW(BrushlessCluster_direction_obj, 2, BrushlessCluster_direction);
MP_DEFINE_CONST_FUN_OBJ_1(BrushlessCluster_load_obj, BrushlessCluster_load);

/***** Binding of Methods *****/
static const mp_rom_map_elem_t Brushless_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Brushless___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&Brushless_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_has_inverses), MP_ROM_PTR(&Brushless_has_inverses_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&Brushless_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&Brushless_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&Brushless_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_u_duty), MP_ROM_PTR(&Brushless_u_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_v_duty), MP_ROM_PTR(&Brushless_v_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_w_duty), MP_ROM_PTR(&Brushless_w_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_duties), MP_ROM_PTR(&Brushless_duties_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&Brushless_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&Brushless_direction_obj) },
};

static const mp_rom_map_elem_t BrushlessCluster_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&BrushlessCluster___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&BrushlessCluster_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&BrushlessCluster_pins_obj) },
    { MP_ROM_QSTR(MP_QSTR_has_inverses), MP_ROM_PTR(&BrushlessCluster_has_inverses_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&BrushlessCluster_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_all), MP_ROM_PTR(&BrushlessCluster_enable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&BrushlessCluster_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_all), MP_ROM_PTR(&BrushlessCluster_disable_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&BrushlessCluster_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_u_duty), MP_ROM_PTR(&BrushlessCluster_u_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_v_duty), MP_ROM_PTR(&BrushlessCluster_v_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_w_duty), MP_ROM_PTR(&BrushlessCluster_w_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_duties), MP_ROM_PTR(&BrushlessCluster_duties_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_duties), MP_ROM_PTR(&BrushlessCluster_all_to_duties_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&BrushlessCluster_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_all_to_phase), MP_ROM_PTR(&BrushlessCluster_all_to_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&BrushlessCluster_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&BrushlessCluster_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&BrushlessCluster_load_obj) },
};

static MP_DEFINE_CONST_DICT(Brushless_locals_dict, Brushless_locals_dict_table);
static MP_DEFINE_CONST_DICT(BrushlessCluster_locals_dict, BrushlessCluster_locals_dict_table);

/***** Class Definition *****/
MP_DEFINE_CONST_OBJ_TYPE(
    Brushless_type,
    MP_QSTR_brushless,
    MP_TYPE_FLAG_NONE,
    make_new, Brushless_make_new,
    print, Brushless_print,
    locals_dict, (mp_obj_dict_t*)&Brushless_locals_dict
);

MP_DEFINE_CONST_OBJ_TYPE(
    BrushlessCluster_type,
    MP_QSTR_brushless_cluster,
    MP_TYPE_FLAG_NONE,
    make_new, BrushlessCluster_make_new,
    print, BrushlessCluster_print,
    locals_dict, (mp_obj_dict_t*)&BrushlessCluster_locals_dict
);

/***** Globals Table *****/
static const mp_map_elem_t brushless_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_brushless) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Brushless), (mp_obj_t)&Brushless_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BrushlessCluster), (mp_obj_t)&BrushlessCluster_type },
};
static MP_DEFINE_CONST_DICT(mp_module_brushless_globals, brushless_globals_table);

/***** Module Definition *****/
const mp_obj_module_t brushless_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_brushless_globals,
};
MP_REGISTER_MODULE(MP_QSTR_brushless, brushless_user_cmodule);
