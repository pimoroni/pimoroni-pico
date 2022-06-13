#include "libraries/breakout_bh1745/breakout_bh1745.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>

using namespace pimoroni;

extern "C" {
#include "breakout_bh1745.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_bh1745_BreakoutBH1745_obj_t {
    mp_obj_base_t base;
    BreakoutBH1745 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_bh1745_BreakoutBH1745_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutBH1745_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BH1745::DEFAULT_I2C_ADDRESS} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_bh1745_BreakoutBH1745_obj_t);
    self->base.type = &breakout_bh1745_BreakoutBH1745_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutBH1745, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutBH1745: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutBH1745_chip_id(mp_obj_t self_in) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bh1745_BreakoutBH1745_obj_t);
    return mp_obj_new_int(self->breakout->get_chip_id());
}

mp_obj_t BreakoutBH1745_manufacturer_id(mp_obj_t self_in) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bh1745_BreakoutBH1745_obj_t);
    return mp_obj_new_int(self->breakout->get_manufacturer());
}

mp_obj_t BreakoutBH1745_rgbc_raw(mp_obj_t self_in) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bh1745_BreakoutBH1745_obj_t);

    const rgbc_t reading = self->breakout->get_rgbc_raw();

    mp_obj_t tuple[4];
    tuple[RGBC_R] = mp_obj_new_int(reading.r);
    tuple[RGBC_G] = mp_obj_new_int(reading.g);
    tuple[RGBC_B] = mp_obj_new_int(reading.b);
    tuple[RGBC_C] = mp_obj_new_int(reading.c);

    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t BreakoutBH1745_rgbc_clamped(mp_obj_t self_in) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bh1745_BreakoutBH1745_obj_t);

    const rgbc_t reading = self->breakout->get_rgb_clamped();

    mp_obj_t tuple[4];
    tuple[RGBC_R] = mp_obj_new_int(reading.r);
    tuple[RGBC_G] = mp_obj_new_int(reading.g);
    tuple[RGBC_B] = mp_obj_new_int(reading.b);
    tuple[RGBC_C] = mp_obj_new_int(reading.c);

    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t BreakoutBH1745_rgbc_scaled(mp_obj_t self_in) {
    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_bh1745_BreakoutBH1745_obj_t);

    const rgbc_t reading = self->breakout->get_rgb_scaled();

    mp_obj_t tuple[4];
    tuple[RGBC_R] = mp_obj_new_int(reading.r);
    tuple[RGBC_G] = mp_obj_new_int(reading.g);
    tuple[RGBC_B] = mp_obj_new_int(reading.b);
    tuple[RGBC_C] = mp_obj_new_int(reading.c);

    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t BreakoutBH1745_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_lower, ARG_upper };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_lower, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_upper, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_bh1745_BreakoutBH1745_obj_t);

    int lower = args[ARG_lower].u_int;
    int upper = args[ARG_upper].u_int;

    if(lower < 0 || lower > 65535) {
        mp_raise_ValueError("lower out of range. Expected 0 to 65535");
    } else if(upper < 0 || upper > 65535) {
        mp_raise_ValueError("upper out of range. Expected 0 to 65535");
    } else {
        self->breakout->set_threshold_low(lower);
        self->breakout->set_threshold_high(upper);
    }

    return mp_const_none;
}

mp_obj_t BreakoutBH1745_measurement_time_ms(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_time };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_time, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_bh1745_BreakoutBH1745_obj_t);

    int measurement_time = args[ARG_time].u_int;

    if(measurement_time < 0 || measurement_time > 65535) {
        mp_raise_ValueError("Time out of range. Expected 0 to 65535");
    } else {
        self->breakout->set_measurement_time_ms(measurement_time);
    }

    return mp_const_none;
}

mp_obj_t BreakoutBH1745_leds(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_led_state };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_led_state, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_bh1745_BreakoutBH1745_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_bh1745_BreakoutBH1745_obj_t);

    bool state = args[ARG_led_state].u_bool;

    self->breakout->set_leds(state);

    return mp_const_none;
}

}
