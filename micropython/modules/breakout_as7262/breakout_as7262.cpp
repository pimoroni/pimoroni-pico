#include "libraries/breakout_as7262/breakout_as7262.hpp"
#include "common/pimoroni_i2c.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_as7262.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_as7262_BreakoutAS7262_obj_t {
    mp_obj_base_t base;
    BreakoutAS7262 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_as7262_BreakoutAS7262_obj_t;



/***** Constructor *****/
mp_obj_t BreakoutAS7262_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_as7262_BreakoutAS7262_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_int };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_as7262_BreakoutAS7262_obj_t);
    self->base.type = &breakout_as7262_BreakoutAS7262_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutAS7262, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_int].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutAS7262: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutAS7262_reset(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);
    self->breakout->reset();

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_device_type(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);
    return mp_obj_new_int(self->breakout->device_type());
}

mp_obj_t BreakoutAS7262_hardware_version(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);
    return mp_obj_new_int(self->breakout->hardware_version());
}

mp_obj_t BreakoutAS7262_firmware_version(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    uint8_t major, minor, sub;
    self->breakout->firmware_version(major, minor, sub);

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int(major);
    tuple[1] = mp_obj_new_float(minor);
    tuple[2] = mp_obj_new_float(sub);
    return mp_obj_new_tuple(3, tuple);
}

mp_obj_t BreakoutAS7262_read(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);
    BreakoutAS7262::reading reading = self->breakout->read();

    mp_obj_t tuple[6];
    tuple[0] = mp_obj_new_float(reading.red);
    tuple[1] = mp_obj_new_float(reading.orange);
    tuple[2] = mp_obj_new_float(reading.yellow);
    tuple[3] = mp_obj_new_float(reading.green);
    tuple[4] = mp_obj_new_float(reading.blue);
    tuple[5] = mp_obj_new_float(reading.violet);
    return mp_obj_new_tuple(6, tuple);
}

mp_obj_t BreakoutAS7262_temperature(mp_obj_t self_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);
    return mp_obj_new_int(self->breakout->temperature());
}

mp_obj_t BreakoutAS7262_set_gain(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    int value = mp_obj_get_int(value_in) & 3;
    
    self->breakout->set_gain((BreakoutAS7262::gain)value);

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_measurement_mode(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    int value = mp_obj_get_int(value_in) & 3;

    self->breakout->set_measurement_mode((BreakoutAS7262::measurement_mode)value);

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_indicator_current(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    int value = mp_obj_get_int(value_in) & 3;

    self->breakout->set_indicator_current((BreakoutAS7262::indicator_current)value);

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_illumination_current(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    int value = mp_obj_get_int(value_in) & 3;

    self->breakout->set_illumination_current((BreakoutAS7262::illumination_current)value);

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_integration_time(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    self->breakout->set_integration_time(mp_obj_get_float(value_in));

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_leds(mp_obj_t self_in, mp_obj_t illumination, mp_obj_t indicator) {
    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_as7262_BreakoutAS7262_obj_t);

    self->breakout->set_leds(
        illumination == mp_const_true,
        indicator == mp_const_true
    );

    return mp_const_none;
}
}