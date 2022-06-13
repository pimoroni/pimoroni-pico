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

mp_obj_t BreakoutAS7262_set_gain(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_gain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_gain, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int gain = args[ARG_gain].u_int;
    if(gain < 0 || gain > 3) {
        mp_raise_ValueError("mode not a valid value. Expected 0 to 3 (X1, X3_7, X16, X64)");
    }
    else {
        breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
        self->breakout->set_gain((BreakoutAS7262::gain)gain);
    }

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_measurement_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int mode = args[ARG_mode].u_int;
    if(mode < 0 || mode > 3) {
        mp_raise_ValueError("mode not a valid value. Expected 0 to 3 (CONT_YGNV, CONT_ROYG, CONT_ROYGBR, ONESHOT)");
    }
    else {
        breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
        self->breakout->set_measurement_mode((BreakoutAS7262::measurement_mode)mode);
    }

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_indicator_current(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_current };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_current, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    int current = args[ARG_current].u_int;
    if(current < 0 || current > 3) {
        mp_raise_ValueError("current not a valid value. Expected 0 to 3 (MA1, MA2, MA4, MA8)");
    }
    else {
        breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
        self->breakout->set_indicator_current((BreakoutAS7262::indicator_current)current);
    }

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_illumination_current(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_current };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_current, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int current = args[ARG_current].u_int;
    if(current < 0 || current > 3) {
        mp_raise_ValueError("current not a valid value. Expected 0 to 3 (MA12, MA25, MA50, MA100)");
    }
    else {  
        breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
        self->breakout->set_illumination_current((BreakoutAS7262::illumination_current)current);
    }

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_integration_time(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_integration_time };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_integration_time, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float integration_time = mp_obj_get_float(args[ARG_integration_time].u_obj);

    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
    self->breakout->set_integration_time(integration_time);

    return mp_const_none;
}

mp_obj_t BreakoutAS7262_set_leds(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_illumination, ARG_indicator };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_illumination, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_indicator, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_as7262_BreakoutAS7262_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_as7262_BreakoutAS7262_obj_t);
    self->breakout->set_leds(args[ARG_illumination].u_bool, args[ARG_indicator].u_bool);

    return mp_const_none;
}
}