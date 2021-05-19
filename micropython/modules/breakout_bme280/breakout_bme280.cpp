#include "drivers/bme280/bme280.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_bme280.h"
#include "pimoroni_i2c.h"

/***** I2C Struct *****/
typedef struct _PimoroniI2C_obj_t {
    mp_obj_base_t base;
    I2C *i2c;
} _PimoroniI2C_obj_t;

/***** Variables Struct *****/
typedef struct _breakout_bme280_BreakoutBME280_obj_t {
    mp_obj_base_t base;
    BME280 *breakout;
} breakout_bme280_BreakoutBME280_obj_t;

/***** Print *****/
void BreakoutBME280_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter    
    breakout_bme280_BreakoutBME280_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bme280_BreakoutBME280_obj_t);
    BME280* breakout = self->breakout;
    mp_print_str(print, "BreakoutBME280(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c()->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_i2c()->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_i2c()->get_scl()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutBME280_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_bme280_BreakoutBME280_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_int };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BME280::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    if(!MP_OBJ_IS_TYPE(args[ARG_i2c].u_obj, &PimoroniI2C_type)) {
        mp_raise_ValueError(MP_ERROR_TEXT("BreakoutBME280: Bad i2C object"));
        return mp_const_none;
    }

    _PimoroniI2C_obj_t *i2c = (_PimoroniI2C_obj_t *)MP_OBJ_TO_PTR(args[ARG_i2c].u_obj);

    self = m_new_obj(breakout_bme280_BreakoutBME280_obj_t);
    self->base.type = &breakout_bme280_BreakoutBME280_type;

    self->breakout = new BME280(i2c->i2c, args[ARG_address].u_int, args[ARG_int].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutBME280: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutBME280_read(mp_obj_t self_in) {
    breakout_bme280_BreakoutBME280_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bme280_BreakoutBME280_obj_t);
    BME280::bme280_reading result = self->breakout->read_forced();

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_float(result.temperature);
    tuple[1] = mp_obj_new_float(result.pressure);
    tuple[2] = mp_obj_new_float(result.humidity);
    return mp_obj_new_tuple(3, tuple);
}

}