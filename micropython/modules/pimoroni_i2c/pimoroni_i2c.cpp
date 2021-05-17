#include "common/pimoroni_i2c.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _PimoroniI2C_obj_t {
    mp_obj_base_t base;
    I2C *i2c;
} _PimoroniI2C_obj_t;


/***** Print *****/
void PimoroniI2C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PimoroniI2C_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniI2C_obj_t);
    I2C* i2c = self->i2c;
    mp_print_str(print, "PimoroniI2C(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((i2c->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_scl()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t PimoroniI2C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PimoroniI2C_obj_t *self = nullptr;

    enum { ARG_sda, ARG_scl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sda, MP_ARG_INT, {.u_int = I2C_DEFAULT_SDA} },
        { MP_QSTR_scl, MP_ARG_INT, {.u_int = I2C_DEFAULT_SCL} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int sda = args[ARG_sda].u_int;
    int scl = args[ARG_scl].u_int;
    int i2c_id = (sda >> 1) & 0b1;  // i2c bus for given SDA pin

    if(!IS_VALID_SDA(i2c_id, sda)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
    }

    if(!IS_VALID_SCL(i2c_id, scl)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
    }

    self = m_new_obj(_PimoroniI2C_obj_t);
    self->base.type = &PimoroniI2C_type;

    self->i2c = new I2C(sda, scl);

    return MP_OBJ_FROM_PTR(self);
}

}