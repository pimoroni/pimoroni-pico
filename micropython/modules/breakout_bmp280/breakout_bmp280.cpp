#include "drivers/bmp280/bmp280.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_bmp280.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_bmp280_BreakoutBMP280_obj_t {
    mp_obj_base_t base;
    BMP280 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_bmp280_BreakoutBMP280_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutBMP280_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_bmp280_BreakoutBMP280_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_int };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BMP280::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_bmp280_BreakoutBMP280_obj_t);
    self->base.type = &breakout_bmp280_BreakoutBMP280_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BMP280, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int, args[ARG_int].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutBMP280: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutBMP280_read(mp_obj_t self_in) {
    breakout_bmp280_BreakoutBMP280_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bmp280_BreakoutBMP280_obj_t);
    BMP280::bmp280_reading result = self->breakout->read();

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(result.temperature);
    tuple[1] = mp_obj_new_float(result.pressure);
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t BreakoutBMP280_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_filter, ARG_standby_time, ARG_os_pressure, ARG_os_temp, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_filter, MP_ARG_INT, { .u_int=BMP280_FILTER_COEFF_2 } },
        { MP_QSTR_standby_time,  MP_ARG_INT, { .u_int=BMP280_ODR_0_5_MS } },
        { MP_QSTR_os_pressure, MP_ARG_INT, { .u_int=BMP280_OS_16X } },
        { MP_QSTR_os_temp, MP_ARG_INT, { .u_int=BMP280_OS_2X } },
        { MP_QSTR_mode, MP_ARG_INT, { .u_int=BMP280_NORMAL_MODE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_bmp280_BreakoutBMP280_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_bmp280_BreakoutBMP280_obj_t);
    self->breakout->configure(
        args[ARG_filter].u_int,
        args[ARG_standby_time].u_int,
        args[ARG_os_pressure].u_int,
        args[ARG_os_temp].u_int,
        args[ARG_mode].u_int
    );

    return mp_const_none;
}

}