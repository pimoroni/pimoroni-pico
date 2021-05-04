#include "../../../libraries/breakout_ltr559/breakout_ltr559.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_ltr559.h"

/***** Variables Struct *****/
typedef struct _breakout_ltr559_BreakoutLTR559_obj_t {
    mp_obj_base_t base;
    BreakoutLTR559 *breakout;
} breakout_ltr559_BreakoutLTR559_obj_t;

/***** Print *****/
void BreakoutLTR559_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_ltr559_BreakoutLTR559_obj_t);
    BreakoutLTR559* breakout = self->breakout;
    mp_print_str(print, "BreakoutLTR559(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutLTR559_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_ltr559_BreakoutLTR559_obj_t *self = nullptr;

    if(n_args == 0) {
        mp_arg_check_num(n_args, n_kw, 0, 0, true);
        self = m_new_obj(breakout_ltr559_BreakoutLTR559_obj_t);
        self->base.type = &breakout_ltr559_BreakoutLTR559_type;
        self->breakout = new BreakoutLTR559();
    }
    else if(n_args == 1) {
        enum { ARG_address };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        self = m_new_obj(breakout_ltr559_BreakoutLTR559_obj_t);
        self->base.type = &breakout_ltr559_BreakoutLTR559_type;
        
        self->breakout = new BreakoutLTR559(args[ARG_address].u_int);
    }
    else {
        enum { ARG_i2c, ARG_address, ARG_sda, ARG_scl, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = BreakoutLTR559::PIN_UNUSED} },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        // Get I2C bus.
        int i2c_id = args[ARG_i2c].u_int;
        if(i2c_id < 0 || i2c_id > 1) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
        }

        int sda = args[ARG_sda].u_int;
        if (!IS_VALID_SDA(i2c_id, sda)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
        }

        int scl = args[ARG_scl].u_int;
        if (!IS_VALID_SCL(i2c_id, scl)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
        }        

        self = m_new_obj(breakout_ltr559_BreakoutLTR559_obj_t);
        self->base.type = &breakout_ltr559_BreakoutLTR559_type;
        
        i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
        self->breakout = new BreakoutLTR559(i2c, args[ARG_address].u_int, sda, scl, args[ARG_interrupt].u_int);
    }

    self->breakout->init();

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutLTR559_part_id(mp_obj_t self_in) {
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_ltr559_BreakoutLTR559_obj_t);
    return mp_obj_new_int(self->breakout->part_id());
}

mp_obj_t BreakoutLTR559_revision_id(mp_obj_t self_in) {
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_ltr559_BreakoutLTR559_obj_t);
    return mp_obj_new_int(self->breakout->revision_id());
}

mp_obj_t BreakoutLTR559_manufacturer_id(mp_obj_t self_in) {
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_ltr559_BreakoutLTR559_obj_t);
    return mp_obj_new_int(self->breakout->manufacturer_id());
}

mp_obj_t BreakoutLTR559_get_reading(mp_obj_t self_in) {
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_ltr559_BreakoutLTR559_obj_t);
    if(self->breakout->get_reading()) {
        const ltr559_reading& data = self->breakout->data;

        mp_obj_t tuple[7];
        tuple[PROXIMITY] = mp_obj_new_int(data.proximity);
        tuple[ALS_0] = mp_obj_new_int(data.als0);
        tuple[ALS_1] = mp_obj_new_int(data.als1);
        tuple[INTEGRATION_TIME] = mp_obj_new_int(data.integration_time);
        tuple[GAIN] = mp_obj_new_int(data.gain);
        tuple[RATIO] = mp_obj_new_float(data.ratio);
        tuple[LUX] = mp_obj_new_int(data.lux);

        return mp_obj_new_tuple(7, tuple);
    }

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_interrupts(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);
    self->breakout->interrupts(args[ARG_light].u_bool, args[ARG_proximity].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_proximity_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_current, ARG_duty_cycle, ARG_pulse_freq, ARG_num_pulses };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_current, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_duty_cycle, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pulse_freq, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_num_pulses, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int current = args[ARG_current].u_int;
    int duty_cycle = args[ARG_duty_cycle].u_int;
    int pulse_freq = args[ARG_pulse_freq].u_int;
    int num_pulses = args[ARG_num_pulses].u_int;

    if(current < 0 || current > 255)
        mp_raise_ValueError("current out of range. Expected 0 to 255");
    else if(duty_cycle < 0 || duty_cycle > 255)
        mp_raise_ValueError("duty_cycle out of range. Expected 0 to 255");
    else if(pulse_freq < 0 || pulse_freq > 255)
        mp_raise_ValueError("pulse_freq out of range. Expected 0 to 255");
    else if(num_pulses < 0 || num_pulses > 255)
        mp_raise_ValueError("num_pulses out of range. Expected 0 to 255");
    else
        self->breakout->proximity_led(current, duty_cycle, pulse_freq, num_pulses);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_light_control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_active, ARG_gain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_active, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_gain, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int gain = args[ARG_gain].u_int;

    if(gain < 0 || gain > 255)
        mp_raise_ValueError("gain out of range. Expected 0 to 255");
    else
        self->breakout->light_control(args[ARG_active].u_bool, gain);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_proximity_control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_active, ARG_saturation_indicator };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_active, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_saturation_indicator, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);
    self->breakout->proximity_control(args[ARG_active].u_bool, args[ARG_saturation_indicator].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_light_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_lower, ARG_upper };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_lower, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_upper, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int lower = args[ARG_lower].u_int;
    int upper = args[ARG_upper].u_int;

    if(lower < 0 || lower > 65535)
        mp_raise_ValueError("lower out of range. Expected 0 to 65535");
    else if(upper < 0 || upper > 65535)
        mp_raise_ValueError("upper out of range. Expected 0 to 65535");
    else
        self->breakout->light_threshold(lower, upper);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_proximity_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_lower, ARG_upper };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_lower, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_upper, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int lower = args[ARG_lower].u_int;
    int upper = args[ARG_upper].u_int;

    if(lower < 0 || lower > 65535)
        mp_raise_ValueError("lower out of range. Expected 0 to 65535");
    else if(upper < 0 || upper > 65535)
        mp_raise_ValueError("upper out of range. Expected 0 to 65535");
    else
        self->breakout->proximity_threshold(lower, upper);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_light_measurement_rate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_integration_time, ARG_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_integration_time, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_rate, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int integration_time = args[ARG_integration_time].u_int;
    int rate = args[ARG_rate].u_int;

    if(integration_time < 0 || integration_time > 65535)
        mp_raise_ValueError("integration_time out of range. Expected 0 to 65535");
    else if(rate < 0 || rate > 65535)
        mp_raise_ValueError("rate out of range. Expected 0 to 65535");
    else
        self->breakout->light_measurement_rate(integration_time, rate);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_proximity_measurement_rate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rate, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int rate = args[ARG_rate].u_int;

    if(rate < 0 || rate > 65535)
        mp_raise_ValueError("rate out of range. Expected 0 to 65535");
    else
        self->breakout->proximity_measurement_rate(rate);

    return mp_const_none;
}

mp_obj_t BreakoutLTR559_proximity_offset(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_offset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_offset, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_ltr559_BreakoutLTR559_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_ltr559_BreakoutLTR559_obj_t);

    int offset = args[ARG_offset].u_int;

    if(offset < 0 || offset > 65535)
        mp_raise_ValueError("offset out of range. Expected 0 to 65535");
    else
        self->breakout->proximity_offset(offset);

    return mp_const_none;
}
}