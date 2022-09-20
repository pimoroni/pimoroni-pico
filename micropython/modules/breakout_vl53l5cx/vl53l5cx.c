#include "vl53l5cx.h"


MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX___del___obj, VL53L5CX___del__);

MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_start_ranging_obj, VL53L5CX_start_ranging);
MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_stop_ranging_obj, VL53L5CX_stop_ranging);

MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_enable_motion_indicator_obj, VL53L5CX_enable_motion_indicator);
MP_DEFINE_CONST_FUN_OBJ_3(VL53L5CX_set_motion_distance_obj, VL53L5CX_set_motion_distance);

MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_i2c_address_obj, VL53L5CX_set_i2c_address);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_ranging_mode_obj, VL53L5CX_set_ranging_mode);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_ranging_frequency_hz_obj, VL53L5CX_set_ranging_frequency_hz);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_resolution_obj, VL53L5CX_set_resolution);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_integration_time_ms_obj, VL53L5CX_set_integration_time_ms);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_sharpener_percent_obj, VL53L5CX_set_sharpener_percent);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_target_order_obj, VL53L5CX_set_target_order);
MP_DEFINE_CONST_FUN_OBJ_2(VL53L5CX_set_power_mode_obj, VL53L5CX_set_power_mode);

MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_data_ready_obj, VL53L5CX_data_ready);
MP_DEFINE_CONST_FUN_OBJ_1(VL53L5CX_get_data_obj, VL53L5CX_get_data);


/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t VL53L5CX_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&VL53L5CX___del___obj) },

    { MP_ROM_QSTR(MP_QSTR_start_ranging), MP_ROM_PTR(&VL53L5CX_start_ranging_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_ranging), MP_ROM_PTR(&VL53L5CX_stop_ranging_obj) },

    { MP_ROM_QSTR(MP_QSTR_enable_motion_indicator), MP_ROM_PTR(&VL53L5CX_enable_motion_indicator_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_motion_distance), MP_ROM_PTR(&VL53L5CX_set_motion_distance_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_i2c_address), MP_ROM_PTR(&VL53L5CX_set_i2c_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ranging_mode), MP_ROM_PTR(&VL53L5CX_set_ranging_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ranging_frequency_hz), MP_ROM_PTR(&VL53L5CX_set_ranging_frequency_hz_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_resolution), MP_ROM_PTR(&VL53L5CX_set_resolution_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_integration_time_ms), MP_ROM_PTR(&VL53L5CX_set_integration_time_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_sharpener_percent), MP_ROM_PTR(&VL53L5CX_set_sharpener_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_target_order), MP_ROM_PTR(&VL53L5CX_set_target_order_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_power_mode), MP_ROM_PTR(&VL53L5CX_set_power_mode_obj) },

    { MP_ROM_QSTR(MP_QSTR_data_ready), MP_ROM_PTR(&VL53L5CX_data_ready_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_data), MP_ROM_PTR(&VL53L5CX_get_data_obj) },
};
STATIC MP_DEFINE_CONST_DICT(VL53L5CX_locals_dict, VL53L5CX_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    VL53L5CX_type,
    MP_QSTR_VL53L5CX,
    MP_TYPE_FLAG_NONE,
    make_new, VL53L5CX_make_new,
    locals_dict, (mp_obj_dict_t*)&VL53L5CX_locals_dict
);
#else
const mp_obj_type_t VL53L5CX_type = {
    { &mp_type_type },
    .name = MP_QSTR_VL53L5CX,
    .make_new = VL53L5CX_make_new,
    .locals_dict = (mp_obj_dict_t*)&VL53L5CX_locals_dict,
};
#endif

/***** Module Globals *****/
STATIC const mp_map_elem_t vl53l5cx_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_vl53l5cx) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VL53L5CX), (mp_obj_t)&VL53L5CX_type },

    { MP_ROM_QSTR(MP_QSTR_TARGET_ORDER_CLOSEST), MP_ROM_INT(VL53L5CX_TARGET_ORDER_CLOSEST) },
    { MP_ROM_QSTR(MP_QSTR_TARGET_ORDER_STRONGEST), MP_ROM_INT(VL53L5CX_TARGET_ORDER_STRONGEST) },

    { MP_ROM_QSTR(MP_QSTR_RESOLUTION_4X4), MP_ROM_INT(VL53L5CX_RESOLUTION_4X4) },
    { MP_ROM_QSTR(MP_QSTR_RESOLUTION_8X8), MP_ROM_INT(VL53L5CX_RESOLUTION_8X8) },

    { MP_ROM_QSTR(MP_QSTR_RANGING_MODE_CONTINUOUS), MP_ROM_INT(VL53L5CX_RANGING_MODE_CONTINUOUS) },
    { MP_ROM_QSTR(MP_QSTR_RANGING_MODE_AUTONOMOUS), MP_ROM_INT(VL53L5CX_RANGING_MODE_AUTONOMOUS) },

    { MP_ROM_QSTR(MP_QSTR_POWER_MODE_SLEEP), MP_ROM_INT(VL53L5CX_POWER_MODE_SLEEP) },
    { MP_ROM_QSTR(MP_QSTR_POWER_MODE_WAKEUP), MP_ROM_INT(VL53L5CX_POWER_MODE_WAKEUP) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_vl53l5cx_globals, vl53l5cx_globals_table);

/***** Module Definition *****/
const mp_obj_module_t vl53l5cx_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_vl53l5cx_globals,
};

/***** Module Registration: as "breakout_vl53l5cx" *****/
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_vl53l5cx, vl53l5cx_user_cmodule, MODULE_VL53L5CX_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_vl53l5cx, vl53l5cx_user_cmodule);
#endif
