#include "breakout_bme280.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutBME280 Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutBME280_read_obj, BreakoutBME280_read);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBME280_configure_obj, 1, BreakoutBME280_configure);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutBME280_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutBME280_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&BreakoutBME280_configure_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutBME280_locals_dict, BreakoutBME280_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_bme280_BreakoutBME280_type,
    MP_QSTR_BreakoutBME280,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutBME280_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutBME280_locals_dict
);
#else
const mp_obj_type_t breakout_bme280_BreakoutBME280_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutBME280,
    .make_new = BreakoutBME280_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutBME280_locals_dict,
};
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_bme280 Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_bme280_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_bme280) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutBME280), (mp_obj_t)&breakout_bme280_BreakoutBME280_type },

    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_OFF), MP_ROM_INT(BME280_FILTER_COEFF_OFF) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_2), MP_ROM_INT(BME280_FILTER_COEFF_2) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_4), MP_ROM_INT(BME280_FILTER_COEFF_4) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_8), MP_ROM_INT(BME280_FILTER_COEFF_8) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_16), MP_ROM_INT(BME280_FILTER_COEFF_16) },

    { MP_ROM_QSTR(MP_QSTR_NO_OVERSAMPLING), MP_ROM_INT(BME280_NO_OVERSAMPLING) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_1X), MP_ROM_INT(BME280_OVERSAMPLING_1X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_2X), MP_ROM_INT(BME280_OVERSAMPLING_2X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_4X), MP_ROM_INT(BME280_OVERSAMPLING_4X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_8X), MP_ROM_INT(BME280_OVERSAMPLING_8X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_16X), MP_ROM_INT(BME280_OVERSAMPLING_16X) },

    { MP_ROM_QSTR(MP_QSTR_SLEEP_MODE), MP_ROM_INT(BME280_SLEEP_MODE) },
    { MP_ROM_QSTR(MP_QSTR_FORCED_MODE), MP_ROM_INT(BME280_FORCED_MODE) },
    { MP_ROM_QSTR(MP_QSTR_NORMAL_MODE), MP_ROM_INT(BME280_NORMAL_MODE) },

    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_0_5_MS), MP_ROM_INT(BME280_STANDBY_TIME_0_5_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_62_5_MS), MP_ROM_INT(BME280_STANDBY_TIME_62_5_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_125_MS), MP_ROM_INT(BME280_STANDBY_TIME_125_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_250_MS), MP_ROM_INT(BME280_STANDBY_TIME_250_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_500_MS), MP_ROM_INT(BME280_STANDBY_TIME_500_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_1000_MS), MP_ROM_INT(BME280_STANDBY_TIME_1000_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_10_MS), MP_ROM_INT(BME280_STANDBY_TIME_10_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_20_MS), MP_ROM_INT(BME280_STANDBY_TIME_20_MS) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_bme280_globals, breakout_bme280_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_bme280_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_bme280_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_bme280, breakout_bme280_user_cmodule, MODULE_BREAKOUT_BME280_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_bme280, breakout_bme280_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////