#include "breakout_bme68x.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutBME68X Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBME68X_read_obj, 1, BreakoutBME68X_read);
MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutBME68X_configure_obj, 1, BreakoutBME68X_configure);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutBME68X_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&BreakoutBME68X_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&BreakoutBME68X_configure_obj) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutBME68X_locals_dict, BreakoutBME68X_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_bme68x_BreakoutBME68X_type,
    MP_QSTR_BreakoutBME68X,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutBME68X_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutBME68X_locals_dict
);
#else
const mp_obj_type_t breakout_bme68x_BreakoutBME68X_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutBME68X,
    .make_new = BreakoutBME68X_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutBME68X_locals_dict,
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_bme68x Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_bme68x_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_bme68x) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutBME68X), (mp_obj_t)&breakout_bme68x_BreakoutBME68X_type },

    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_OFF), MP_ROM_INT(BME68X_FILTER_OFF) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_1), MP_ROM_INT(BME68X_FILTER_SIZE_1) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_3), MP_ROM_INT(BME68X_FILTER_SIZE_3) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_7), MP_ROM_INT(BME68X_FILTER_SIZE_7) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_15), MP_ROM_INT(BME68X_FILTER_SIZE_15) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_31), MP_ROM_INT(BME68X_FILTER_SIZE_31) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_63), MP_ROM_INT(BME68X_FILTER_SIZE_63) },
    { MP_ROM_QSTR(MP_QSTR_FILTER_COEFF_127), MP_ROM_INT(BME68X_FILTER_SIZE_127) },

    { MP_ROM_QSTR(MP_QSTR_NO_OVERSAMPLING), MP_ROM_INT(BME68X_OS_NONE) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_1X), MP_ROM_INT(BME68X_OS_1X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_2X), MP_ROM_INT(BME68X_OS_2X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_4X), MP_ROM_INT(BME68X_OS_4X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_8X), MP_ROM_INT(BME68X_OS_8X) },
    { MP_ROM_QSTR(MP_QSTR_OVERSAMPLING_16X), MP_ROM_INT(BME68X_OS_16X) },

/*  TODO add MicroPython support for alternate reading modes?
    { MP_ROM_QSTR(MP_QSTR_SLEEP_MODE), MP_ROM_INT(BME68X_SLEEP_MODE) },
    { MP_ROM_QSTR(MP_QSTR_FORCED_MODE), MP_ROM_INT(BME68X_FORCED_MODE) },
    { MP_ROM_QSTR(MP_QSTR_PARALLEL_MODE), MP_ROM_INT(BME68X_PARALLEL_MODE) },
    { MP_ROM_QSTR(MP_QSTR_SEQUENTIAL_MODE), MP_ROM_INT(BME68X_SEQUENTIAL_MODE) },
*/

    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_OFF), MP_ROM_INT(BME68X_ODR_NONE) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_0_59_MS), MP_ROM_INT(BME68X_ODR_0_59_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_62_5_MS), MP_ROM_INT(BME68X_ODR_62_5_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_125_MS), MP_ROM_INT(BME68X_ODR_125_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_250_MS), MP_ROM_INT(BME68X_ODR_250_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_500_MS), MP_ROM_INT(BME68X_ODR_500_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_1000_MS), MP_ROM_INT(BME68X_ODR_1000_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_10_MS), MP_ROM_INT(BME68X_ODR_10_MS) },
    { MP_ROM_QSTR(MP_QSTR_STANDBY_TIME_20_MS), MP_ROM_INT(BME68X_ODR_20_MS) },

    { MP_ROM_QSTR(MP_QSTR_STATUS_GAS_VALID), MP_ROM_INT(BME68X_GASM_VALID_MSK) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_HEATER_STABLE), MP_ROM_INT(BME68X_HEAT_STAB_MSK) },

    { MP_ROM_QSTR(MP_QSTR_I2C_ADDRESS_DEFAULT), MP_ROM_INT(BME68X_I2C_ADDR_LOW) },
    { MP_ROM_QSTR(MP_QSTR_I2C_ADDRESS_ALT), MP_ROM_INT(BME68X_I2C_ADDR_HIGH) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_bme68x_globals, breakout_bme68x_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_bme68x_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_bme68x_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_bme68x, breakout_bme68x_user_cmodule, MODULE_BREAKOUT_BME68X_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_bme68x, breakout_bme68x_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
