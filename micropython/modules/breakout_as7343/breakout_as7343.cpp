#include "drivers/as7343/as7343.hpp"
#include "common/pimoroni_i2c.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_as7343.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_as7343_BreakoutAS7343_obj_t {
    mp_obj_base_t base;
    AS7343 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_as7343_BreakoutAS7343_obj_t;



/***** Constructor *****/
mp_obj_t BreakoutAS7343_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_as7343_BreakoutAS7343_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_int };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_as7343_BreakoutAS7343_obj_t);
    self->base.type = &breakout_as7343_BreakoutAS7343_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(AS7343, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_int].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("BreakoutAS7343: breakout not found when initialising"));
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutAS7343_reset(mp_obj_t self_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);
    self->breakout->reset();

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_version(mp_obj_t self_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    uint8_t aux_id, revision_id, hardware_id;
    self->breakout->get_version(aux_id, revision_id, hardware_id);

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int(aux_id);
    tuple[1] = mp_obj_new_int(revision_id);
    tuple[2] = mp_obj_new_int(hardware_id);
    return mp_obj_new_tuple(3, tuple);
}

mp_obj_t BreakoutAS7343_read(mp_obj_t self_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);
    AS7343::reading reading = self->breakout->read();

    mp_obj_t tuple[12];
    tuple[0] = mp_obj_new_float(reading.FZ);
    tuple[1] = mp_obj_new_float(reading.FY);
    tuple[2] = mp_obj_new_float(reading.FXL);
    tuple[3] = mp_obj_new_float(reading.NIR);

    tuple[4] = mp_obj_new_float(reading.F2);
    tuple[5] = mp_obj_new_float(reading.F3);
    tuple[6] = mp_obj_new_float(reading.F4);
    tuple[7] = mp_obj_new_float(reading.F6);

    tuple[8] = mp_obj_new_float(reading.F1);
    tuple[9] = mp_obj_new_float(reading.F5);
    tuple[10] = mp_obj_new_float(reading.F7);
    tuple[11] = mp_obj_new_float(reading.F8);

    return mp_obj_new_tuple(12, tuple);
}

mp_obj_t BreakoutAS7343_set_gain(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_gain(mp_obj_get_int(value_in));

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_set_measurement_time(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_measurement_time(mp_obj_get_float(value_in));

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_set_integration_time(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_integration_time(mp_obj_get_float(value_in));

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_set_illumination_current(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_illumination_current(mp_obj_get_int(value_in));

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_set_illumination_led(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_illumination_led(value_in == mp_const_true);

    return mp_const_none;
}

mp_obj_t BreakoutAS7343_set_channels(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7343_BreakoutAS7343_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7343_BreakoutAS7343_obj_t);

    self->breakout->set_channels((AS7343::channel_count)mp_obj_get_int(value_in));

    return mp_const_none;
}
}