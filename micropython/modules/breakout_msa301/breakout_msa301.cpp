#include "libraries/breakout_msa301/breakout_msa301.hpp"
#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_msa301.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_msa301_BreakoutMSA301_obj_t {
    mp_obj_base_t base;
    BreakoutMSA301 *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_msa301_BreakoutMSA301_obj_t;

/***** Constructor *****/
mp_obj_t BreakoutMSA301_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_msa301_BreakoutMSA301_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_msa301_BreakoutMSA301_obj_t);
    self->base.type = &breakout_msa301_BreakoutMSA301_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutMSA301, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutMSA301: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutMSA301_part_id(mp_obj_t self_in) {
    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_msa301_BreakoutMSA301_obj_t);
    return mp_obj_new_int(self->breakout->part_id());
}

mp_obj_t BreakoutMSA301_get_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_axis, ARG_sample_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_axis, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sample_count, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int sample_count = args[ARG_sample_count].u_int;
    if(sample_count < 0 || sample_count > 255)
        mp_raise_ValueError("sample_count out of range. Expected 0 to 255");
    else {
        float value = 0.0f;
        switch(args[ARG_axis].u_int) {
        case MSA_AXIS_X:
            value = self->breakout->get_axis(BreakoutMSA301::X, sample_count);
            break;
        case MSA_AXIS_Y:
            value = self->breakout->get_axis(BreakoutMSA301::Y, sample_count);
            break;
        case MSA_AXIS_Z:
            value = self->breakout->get_axis(BreakoutMSA301::Z, sample_count);
            break;
        default:
            mp_raise_ValueError("axis out of range. Expected 0 to 2 (X, Y, Z)");
            break;
        }

        return mp_obj_new_float(value);
    }

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_get_x_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_sample_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sample_count, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int sample_count = args[ARG_sample_count].u_int;
    if(sample_count < 0 || sample_count > 255)
        mp_raise_ValueError("sample_count out of range. Expected 0 to 255");
    else
        return mp_obj_new_float(self->breakout->get_x_axis(sample_count));

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_get_y_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_sample_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sample_count, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int sample_count = args[ARG_sample_count].u_int;
    if(sample_count < 0 || sample_count > 255)
        mp_raise_ValueError("sample_count out of range. Expected 0 to 255");
    else
        return mp_obj_new_float(self->breakout->get_y_axis(sample_count));

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_get_z_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_sample_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sample_count, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int sample_count = args[ARG_sample_count].u_int;
    if(sample_count < 0 || sample_count > 255)
        mp_raise_ValueError("sample_count out of range. Expected 0 to 255");
    else
        return mp_obj_new_float(self->breakout->get_z_axis(sample_count));

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_get_orientation(mp_obj_t self_in) {
    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_msa301_BreakoutMSA301_obj_t);
    return mp_obj_new_int(self->breakout->get_orientation());
}

mp_obj_t BreakoutMSA301_set_power_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_power_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_power_mode, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int power_mode = args[ARG_power_mode].u_int;
    if(power_mode < 0 || power_mode > 255)
        mp_raise_ValueError("power_mode out of range. Expected 0 to 2 (NORMAL, LOW, SUSPEND)");
    else
        self->breakout->set_power_mode((BreakoutMSA301::PowerMode)power_mode);

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_set_range_and_resolution(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_range, ARG_resolution };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_range, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_resolution, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int range = args[ARG_range].u_int;
    int resolution = args[ARG_resolution].u_int;
    if(range < 0 || range > 3)
        mp_raise_ValueError("range out of range. Expected 0 to 3 (G_2, G_4, G_8, G_16)");
    if(resolution < 0 || resolution > 3)
        mp_raise_ValueError("resolution out of range. Expected 0 to 3 (BITS_14, BITS_12, BITS_10, BITS_8)");
    else
        self->breakout->set_range_and_resolution((BreakoutMSA301::Range)range, (BreakoutMSA301::Resolution)(resolution << 2));

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_set_axis_polarity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_polarity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_polarity, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int polarity = args[ARG_polarity].u_int;
    if(polarity < 0 || polarity > 15)
        mp_raise_ValueError("polarity out of range. Expected 0 or the bitwise combination of 1 (INVERT_X), 2 (INVERT_Y), 4 (INVERT_Z), or 8 (XY_SWAP)");
    else
        self->breakout->set_axis_polarity(polarity);

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_disable_all_interrupts(mp_obj_t self_in) {
    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_msa301_BreakoutMSA301_obj_t);
    self->breakout->disable_all_interrupts();

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_enable_interrupts(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_interrupts };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_interrupts, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int interrupts = args[ARG_interrupts].u_int;

    uint32_t mask = BreakoutMSA301::ACTIVE |
                    BreakoutMSA301::NEW_DATA |
                    BreakoutMSA301::FREEFALL |
                    BreakoutMSA301::ORIENTATION |
                    BreakoutMSA301::SINGLE_TAP |
                    BreakoutMSA301::DOUBLE_TAP;
    if(interrupts < 0 || (interrupts & mask) == 0)
        mp_raise_ValueError("interrupts out of range. Expected 0 or the bitwise combination of 1 (X_ACTIVE), 2 (Y_ACTIVE), 4 (Z_ACTIVE), 16 (DOUBLE_TAP), 32 (SINGLE_TAP), 64 (ORIENTATION), 2048 (FREEFALL), 4096 (NEW_DATA)");
    else
        self->breakout->enable_interrupts(interrupts);

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_set_interrupt_latch(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_latch_period, ARG_reset_latched };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_latch_period, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_reset_latched, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int latch_period = args[ARG_latch_period].u_int;
    bool reset_latched = args[ARG_latch_period].u_bool;
    BreakoutMSA301::InterruptLatchPeriod period;
    switch(latch_period) {
        case MSA_LATCH_1MS:     period = BreakoutMSA301::LATCH_1MS;     break;
        case MSA_LATCH_2MS:     period = BreakoutMSA301::LATCH_2MS;     break;
        case MSA_LATCH_25MS:    period = BreakoutMSA301::LATCH_25MS;    break;
        case MSA_LATCH_50MS:    period = BreakoutMSA301::LATCH_50MS;    break;
        case MSA_LATCH_100MS:   period = BreakoutMSA301::LATCH_100MS;   break;
        case MSA_LATCH_250MS:   period = BreakoutMSA301::LATCH_250MS;   break;
        case MSA_LATCH_500MS:   period = BreakoutMSA301::LATCH_500MS;   break;
        case MSA_LATCH_1S:      period = BreakoutMSA301::LATCH_1S;      break;
        case MSA_LATCH_2S:      period = BreakoutMSA301::LATCH_2S;      break;
        case MSA_LATCH_4S:      period = BreakoutMSA301::LATCH_4S;      break;
        case MSA_LATCH_8S:      period = BreakoutMSA301::LATCH_8S;      break;
        default:
            mp_raise_ValueError("latch_period out of range. Expected 0 to 10 (LATCH_1MS, LATCH_2MS, LATCH_25MS, LATCH_50MS, LATCH_100MS, LATCH_250MS, LATCH_500MS, LATCH_1S, LATCH_2S, LATCH_4S, or LATCH_8S)");
            break;
    }
    self->breakout->set_interrupt_latch(period, reset_latched);

    return mp_const_none;
}

mp_obj_t BreakoutMSA301_read_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_interrupt, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_msa301_BreakoutMSA301_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_msa301_BreakoutMSA301_obj_t);

    int interrupt = args[ARG_interrupt].u_int;
    switch(interrupt) {
        case BreakoutMSA301::NEW_DATA:
        case BreakoutMSA301::FREEFALL:
        case BreakoutMSA301::ACTIVE:
        case BreakoutMSA301::DOUBLE_TAP:
        case BreakoutMSA301::SINGLE_TAP:
        case BreakoutMSA301::ORIENTATION:
            return mp_obj_new_bool(self->breakout->read_interrupt((BreakoutMSA301::Interrupt)interrupt));
        default:
            mp_raise_ValueError("interrupt not valid. Expected 7 (ACTIVE), 16 (DOUBLE_TAP), 32 (SINGLE_TAP), 64 (ORIENTATION), 2048 (FREEFALL), 4096 (NEW_DATA)");
            break;
    }

    return mp_const_none;
}
}