#include "libraries/breakout_mics6814/breakout_mics6814.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;

extern "C" {
#include "breakout_mics6814.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_mics6814_BreakoutMICS6814_obj_t {
    mp_obj_base_t base;
    BreakoutMICS6814 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_mics6814_BreakoutMICS6814_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutMICS6814_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutMICS6814::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_mics6814_BreakoutMICS6814_obj_t);
    self->base.type = &breakout_mics6814_BreakoutMICS6814_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutMICS6814, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int, args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutMICS6814: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutMICS6814_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_mics6814_BreakoutMICS6814_obj_t);

    self->breakout->set_address(args[ARG_address].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_get_adc_vref(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->get_adc_vref());
}

mp_obj_t BreakoutMICS6814_set_adc_vref(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_vref };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_vref, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_mics6814_BreakoutMICS6814_obj_t);

    float vref = mp_obj_get_float(args[ARG_vref].u_obj);
    self->breakout->set_adc_vref(vref);

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_mics6814_BreakoutMICS6814_obj_t);

    float brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->breakout->set_brightness(brightness);

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_mics6814_BreakoutMICS6814_obj_t);

    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        self->breakout->set_led(r, g, b);

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_set_heater(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_on };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_on, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_mics6814_BreakoutMICS6814_obj_t);

    self->breakout->set_heater(args[ARG_on].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_disable_heater(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    self->breakout->disable_heater();

    return mp_const_none;
}

mp_obj_t BreakoutMICS6814_get_raw_ref(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->get_raw_ref());
}

mp_obj_t BreakoutMICS6814_get_raw_red(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->get_raw_red());
}

mp_obj_t BreakoutMICS6814_get_raw_nh3(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->get_raw_nh3());
}

mp_obj_t BreakoutMICS6814_get_raw_oxd(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->get_raw_oxd());
}

mp_obj_t BreakoutMICS6814_read_all(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    BreakoutMICS6814::Reading reading = self->breakout->read_all();

    mp_obj_t tuple[4];
    tuple[REF] = mp_obj_new_float(reading.ref);
    tuple[REDUCING] = mp_obj_new_float(reading.reducing);
    tuple[NH3] = mp_obj_new_float(reading.nh3);
    tuple[OXIDISING] = mp_obj_new_float(reading.oxidising);

    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t BreakoutMICS6814_read_ref(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->read_ref());
}

mp_obj_t BreakoutMICS6814_read_reducing(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->read_reducing());
}

mp_obj_t BreakoutMICS6814_read_nh3(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->read_nh3());
}

mp_obj_t BreakoutMICS6814_read_oxidising(mp_obj_t self_in) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    return mp_obj_new_float(self->breakout->read_oxidising());
}
}