#include "../../../pimoroni-pico/libraries/breakout_encoder/breakout_encoder.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_encoder.h"

/***** Variables Struct *****/
typedef struct _breakout_encoder_BreakoutEncoder_obj_t {
    mp_obj_base_t base;
    BreakoutEncoder *breakout;
} breakout_encoder_BreakoutEncoder_obj_t;

/***** Print *****/
void BreakoutEncoder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_BreakoutEncoder_obj_t);
    BreakoutEncoder* breakout = self->breakout;
    mp_print_str(print, "BreakoutEncoder(");

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
mp_obj_t BreakoutEncoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_encoder_BreakoutEncoder_obj_t *self = nullptr;

    if(n_args == 0) {
        mp_arg_check_num(n_args, n_kw, 0, 0, true);
        self = m_new_obj(breakout_encoder_BreakoutEncoder_obj_t);
        self->base.type = &breakout_encoder_BreakoutEncoder_type;
        self->breakout = new BreakoutEncoder();
    }
    else if(n_args == 1) {
        enum { ARG_address };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        self = m_new_obj(breakout_encoder_BreakoutEncoder_obj_t);
        self->base.type = &breakout_encoder_BreakoutEncoder_type;

        self->breakout = new BreakoutEncoder(args[ARG_address].u_int);
    }
    else {
        enum { ARG_i2c, ARG_address, ARG_sda, ARG_scl, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = BreakoutEncoder::PIN_UNUSED} },
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

        self = m_new_obj(breakout_encoder_BreakoutEncoder_obj_t);
        self->base.type = &breakout_encoder_BreakoutEncoder_type;

        i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
        self->breakout = new BreakoutEncoder(i2c, args[ARG_address].u_int, sda, scl, args[ARG_interrupt].u_int);
    }

    self->breakout->init();

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutEncoder_set_addr(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_BreakoutEncoder_obj_t);

    self->breakout->set_addr(args[ARG_address].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutEncoder_get_direction(mp_obj_t self_in) {
    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_BreakoutEncoder_obj_t);
    return mp_obj_new_bool(self->breakout->get_direction());
}

mp_obj_t BreakoutEncoder_set_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_clockwise };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_clockwise, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_BreakoutEncoder_obj_t);

    self->breakout->set_direction(args[ARG_clockwise].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutEncoder_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_BreakoutEncoder_obj_t);

    float brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->breakout->set_brightness((uint8_t)(brightness * 255.0f));

    return mp_const_none;
}

mp_obj_t BreakoutEncoder_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_BreakoutEncoder_obj_t);

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

mp_obj_t BreakoutEncoder_available(mp_obj_t self_in) {
    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_BreakoutEncoder_obj_t);
    return mp_obj_new_bool(self->breakout->available());
}

mp_obj_t BreakoutEncoder_read(mp_obj_t self_in) {
    breakout_encoder_BreakoutEncoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_BreakoutEncoder_obj_t);
    return mp_obj_new_int(self->breakout->read());
}
}