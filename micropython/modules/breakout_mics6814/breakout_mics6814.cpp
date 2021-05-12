#include "../../../libraries/breakout_mics6814/breakout_mics6814.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_mics6814.h"

/***** Variables Struct *****/
typedef struct _breakout_mics6814_BreakoutMICS6814_obj_t {
    mp_obj_base_t base;
    BreakoutMICS6814 *breakout;
} breakout_mics6814_BreakoutMICS6814_obj_t;

/***** Print *****/
void BreakoutMICS6814_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_mics6814_BreakoutMICS6814_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_mics6814_BreakoutMICS6814_obj_t);
    BreakoutMICS6814* breakout = self->breakout;
    mp_print_str(print, "BreakoutMICS6814(");

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
mp_obj_t BreakoutMICS6814_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_mics6814_BreakoutMICS6814_obj_t *self = nullptr;

    if(n_args == 0) {
        mp_arg_check_num(n_args, n_kw, 0, 0, true);
        self = m_new_obj(breakout_mics6814_BreakoutMICS6814_obj_t);
        self->base.type = &breakout_mics6814_BreakoutMICS6814_type;
        self->breakout = new BreakoutMICS6814();
    }
    else if(n_args == 1) {
        enum { ARG_address };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        self = m_new_obj(breakout_mics6814_BreakoutMICS6814_obj_t);
        self->base.type = &breakout_mics6814_BreakoutMICS6814_type;

        self->breakout = new BreakoutMICS6814(args[ARG_address].u_int);
    }
    else {
        enum { ARG_i2c, ARG_address, ARG_sda, ARG_scl, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = BreakoutMICS6814::PIN_UNUSED} },
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

        self = m_new_obj(breakout_mics6814_BreakoutMICS6814_obj_t);
        self->base.type = &breakout_mics6814_BreakoutMICS6814_type;

        i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
        self->breakout = new BreakoutMICS6814(i2c, args[ARG_address].u_int, sda, scl, args[ARG_interrupt].u_int);
    }

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "MICS6814 breakout not found when initialising");
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