#include "libraries/breakout_sgp30/breakout_sgp30.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_sgp30.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_sgp30_BreakoutSGP30_obj_t {
    mp_obj_base_t base;
    BreakoutSGP30 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_sgp30_BreakoutSGP30_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutSGP30_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = nullptr;

    enum { ARG_i2c };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_sgp30_BreakoutSGP30_obj_t);
    self->base.type = &breakout_sgp30_BreakoutSGP30_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutSGP30, (pimoroni::I2C *)(self->i2c->i2c));

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutSGP30: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutSGP30_retrieve_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    return mp_obj_new_bool(self->breakout->retrieve_unique_id());
}

mp_obj_t BreakoutSGP30_get_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    uint64_t id = self->breakout->get_unique_id();

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int((id >> 32) & 0xffff);
    tuple[1] = mp_obj_new_int((id >> 16) & 0xffff);
    tuple[2] = mp_obj_new_int(id & 0xffff);
    return mp_obj_new_tuple(3, tuple);
}

mp_obj_t BreakoutSGP30_start_measurement(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_wait_for_setup };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_wait_for_setup, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->start_measurement(args[ARG_wait_for_setup].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_air_quality(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);

    uint16_t eCO2 = 0, TVOC = 0;
    if(self->breakout->get_air_quality(&eCO2, &TVOC)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(eCO2);
        tuple[1] = mp_obj_new_int(TVOC);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_air_quality_raw(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);

    uint16_t rawH2 = 0, rawEthanol = 0;
    if(self->breakout->get_air_quality_raw(&rawH2, &rawEthanol)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(rawH2);
        tuple[1] = mp_obj_new_int(rawEthanol);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;
}

mp_obj_t BreakoutSGP30_soft_reset(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->soft_reset();

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_baseline(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    
    uint16_t eCO2 = 0, TVOC = 0;
    if(self->breakout->get_baseline(&eCO2, &TVOC)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(eCO2);
        tuple[1] = mp_obj_new_int(TVOC);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_set_baseline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_eco2, ARG_tvoc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_eco2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_tvoc, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->set_baseline(args[ARG_eco2].u_int, args[ARG_tvoc].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_set_humidity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_absolute_humidity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_absolute_humidity, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->set_humidity(args[ARG_absolute_humidity].u_int);

    return mp_const_none;
}
}