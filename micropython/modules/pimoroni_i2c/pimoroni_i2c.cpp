#include "common/pimoroni_i2c.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "pimoroni_i2c.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"


_PimoroniI2C_obj_t*  PimoroniI2C_from_machine_i2c_or_native(mp_obj_t i2c_obj) {
    if(MP_OBJ_IS_TYPE(i2c_obj, &PimoroniI2C_type)) {
        return (_PimoroniI2C_obj_t *)MP_OBJ_TO_PTR(i2c_obj);
    } else if(MP_OBJ_IS_TYPE(i2c_obj, &machine_i2c_type)) {
        _PimoroniI2C_obj_t *pimoroni_i2c = m_new_obj(_PimoroniI2C_obj_t);
        machine_i2c_obj_t *machine_i2c = (machine_i2c_obj_t *)MP_OBJ_TO_PTR(i2c_obj);
        pimoroni_i2c = m_new_obj(_PimoroniI2C_obj_t);
        pimoroni_i2c->base.type = &PimoroniI2C_type;

        pimoroni_i2c->i2c = m_new_class(I2C, machine_i2c->sda, machine_i2c->scl, machine_i2c->freq);
        return pimoroni_i2c;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bad I2C object"));
        return nullptr;
    }
}

void PimoroniI2C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PimoroniI2C_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniI2C_obj_t);
    I2C* i2c = (I2C*)self->i2c;
    mp_print_str(print, "PimoroniI2C(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((i2c->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(i2c->get_scl()), PRINT_REPR);

    mp_print_str(print, ")");
}

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

    self = m_new_obj(_PimoroniI2C_obj_t);
    self->base.type = &PimoroniI2C_type;

    self->i2c = m_new_class(I2C, sda, scl, baud);

    return MP_OBJ_FROM_PTR(self);
}

// Reimplementation of the RP2 port's machine_i2c_transfer_single in terms of Pimoroni I2C
// https://github.com/micropython/micropython/blob/1fb01bd6c5dc350f3c617ca8edae8dea9e5516ae/ports/rp2/machine_i2c.c#L123
int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    _PimoroniI2C_obj_t *self = (_PimoroniI2C_obj_t *)self_in;
    I2C *i2c = (I2C*)self->i2c;
    int ret;
    bool nostop = !(flags & MP_MACHINE_I2C_FLAG_STOP);
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        ret = i2c_read_blocking(i2c->get_i2c(), addr, buf, len, nostop);
    } else {
        if (len == 0) {
            // Workaround issue with hardware I2C not accepting zero-length writes.
            mp_machine_soft_i2c_obj_t soft_i2c = {
                .base = { &mp_machine_soft_i2c_type },
                .us_delay = 500000 / i2c->get_baudrate() + 1,
                .us_timeout = 50000,
                .scl = i2c->get_scl(),
                .sda = i2c->get_sda(),
            };
            mp_machine_i2c_buf_t bufs = {
                .len = len,
                .buf = buf,
            };
            mp_hal_pin_open_drain(i2c->get_scl());
            mp_hal_pin_open_drain(i2c->get_sda());
            ret = mp_machine_soft_i2c_transfer(&soft_i2c.base, addr, 1, &bufs, flags);
            gpio_set_function(i2c->get_scl(), GPIO_FUNC_I2C);
            gpio_set_function(i2c->get_sda(), GPIO_FUNC_I2C);
            return ret;
        } else {
            ret = i2c_write_blocking(i2c->get_i2c(), addr, buf, len, nostop);
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