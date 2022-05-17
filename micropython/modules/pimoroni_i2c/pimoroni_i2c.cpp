#include "common/pimoroni_i2c.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "pimoroni_i2c.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"
#include "hardware/i2c.h"

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

/***** Destructor ******/
mp_obj_t PimoroniI2C___del__(mp_obj_t self_in) {
    _PimoroniI2C_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniI2C_obj_t);
    delete self->i2c;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t PimoroniI2C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PimoroniI2C_obj_t *self = nullptr;

    enum { ARG_sda, ARG_scl, ARG_baudrate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sda, MP_ARG_INT, {.u_int = I2C_DEFAULT_SDA} },
        { MP_QSTR_scl, MP_ARG_INT, {.u_int = I2C_DEFAULT_SCL} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = I2C_DEFAULT_BAUDRATE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int sda = args[ARG_sda].u_int;
    int scl = args[ARG_scl].u_int;
    int baud = args[ARG_baudrate].u_int;
    int i2c_id = (sda >> 1) & 0b1;  // i2c bus for given SDA pin

    if(!IS_VALID_SDA(i2c_id, sda)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
    }

    if(!IS_VALID_SCL(i2c_id, scl)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
    }

    self = m_new_obj_with_finaliser(_PimoroniI2C_obj_t);
    self->base.type = &PimoroniI2C_type;

    self->i2c = new I2C(sda, scl, baud);

    return MP_OBJ_FROM_PTR(self);
}

// Reimplementation of the RP2 port's machine_i2c_transfer_single in terms of Pimoroni I2C
// https://github.com/micropython/micropython/blob/1fb01bd6c5dc350f3c617ca8edae8dea9e5516ae/ports/rp2/machine_i2c.c#L123
int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    _PimoroniI2C_obj_t *self = (_PimoroniI2C_obj_t *)self_in;
    int ret;
    bool nostop = !(flags & MP_MACHINE_I2C_FLAG_STOP);
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        ret = i2c_read_blocking(self->i2c->get_i2c(), addr, buf, len, nostop);
    } else {
        if (len == 0) {
            // Workaround issue with hardware I2C not accepting zero-length writes.
            mp_machine_soft_i2c_obj_t soft_i2c = {
                .base = { &mp_machine_soft_i2c_type },
                .us_delay = 500000 / self->i2c->get_baudrate() + 1,
                .us_timeout = 50000,
                .scl = self->i2c->get_scl(),
                .sda = self->i2c->get_sda(),
            };
            mp_machine_i2c_buf_t bufs = {
                .len = len,
                .buf = buf,
            };
            mp_hal_pin_open_drain(self->i2c->get_scl());
            mp_hal_pin_open_drain(self->i2c->get_sda());
            ret = mp_machine_soft_i2c_transfer(&soft_i2c.base, addr, 1, &bufs, flags);
            gpio_set_function(self->i2c->get_scl(), GPIO_FUNC_I2C);
            gpio_set_function(self->i2c->get_sda(), GPIO_FUNC_I2C);
            return ret;
        } else {
            ret = i2c_write_blocking(self->i2c->get_i2c(), addr, buf, len, nostop);
        }
    }
    if (ret < 0) {
        if (ret == PICO_ERROR_TIMEOUT) {
            return -MP_ETIMEDOUT;
        } else {
            return -MP_EIO;
        }
    } else {
        return ret;
    }
}

}