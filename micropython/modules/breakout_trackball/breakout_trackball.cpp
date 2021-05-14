#include "libraries/breakout_trackball/breakout_trackball.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_trackball.h"

/***** Variables Struct *****/
typedef struct _breakout_trackball_BreakoutTrackball_obj_t {
    mp_obj_base_t base;
    BreakoutTrackball *breakout;
} breakout_trackball_BreakoutTrackball_obj_t;

/***** Print *****/
void BreakoutTrackball_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter    
    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_trackball_BreakoutTrackball_obj_t);
    BreakoutTrackball* breakout = self->breakout;
    mp_print_str(print, "BreakoutTrackball(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", address = 0x");
    char buf[3];
    sprintf(buf, "%02X", breakout->get_address());
    mp_print_str(print, buf);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutTrackball_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_trackball_BreakoutTrackball_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address, ARG_sda, ARG_scl, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutTrackball::DEFAULT_I2C_ADDRESS} },
        { MP_QSTR_sda, MP_ARG_INT, {.u_int = 20} },
        { MP_QSTR_scl, MP_ARG_INT, {.u_int = 21} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = BreakoutTrackball::PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = args[ARG_i2c].u_int;
    int sda = args[ARG_sda].u_int;
    int scl = args[ARG_scl].u_int;

    if(i2c_id == -1) {
        i2c_id = (sda >> 1) & 0b1;  // If no i2c specified, choose the one for the given SDA pin
    }
    if(i2c_id < 0 || i2c_id > 1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    if(!IS_VALID_SDA(i2c_id, sda)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
    }

    if(!IS_VALID_SCL(i2c_id, scl)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
    }

    self = m_new_obj(breakout_trackball_BreakoutTrackball_obj_t);
    self->base.type = &breakout_trackball_BreakoutTrackball_type;

    i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
    self->breakout = new BreakoutTrackball(i2c, args[ARG_address].u_int, sda, scl, args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "Trackball breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutTrackball_change_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    self->breakout->change_address(args[ARG_address].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_enable_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_interrupt, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    self->breakout->enable_interrupt(args[ARG_interrupt].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_get_interrupt(mp_obj_t self_in) {
    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_trackball_BreakoutTrackball_obj_t);
    return mp_obj_new_bool(self->breakout->get_interrupt());
}


mp_obj_t BreakoutTrackball_set_rgbw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;
    int w = args[ARG_w].u_int;

    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else if(w < 0 || w > 255)
        mp_raise_ValueError("w out of range. Expected 0 to 255");
    else
        self->breakout->set_rgbw(r, g, b, w);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_set_red(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    int value = args[ARG_value].u_int;

    if(value < 0 || value > 255)
        mp_raise_ValueError("value out of range. Expected 0 to 255");
    else
        self->breakout->set_red(value);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_set_green(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    int value = args[ARG_value].u_int;

    if(value < 0 || value > 255)
        mp_raise_ValueError("value out of range. Expected 0 to 255");
    else
        self->breakout->set_green(value);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_set_blue(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    int value = args[ARG_value].u_int;

    if(value < 0 || value > 255)
        mp_raise_ValueError("value out of range. Expected 0 to 255");
    else
        self->breakout->set_blue(value);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_set_white(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_trackball_BreakoutTrackball_obj_t);

    int value = args[ARG_value].u_int;

    if(value < 0 || value > 255)
        mp_raise_ValueError("value out of range. Expected 0 to 255");
    else
        self->breakout->set_white(value);

    return mp_const_none;
}

mp_obj_t BreakoutTrackball_read(mp_obj_t self_in) {
    breakout_trackball_BreakoutTrackball_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_trackball_BreakoutTrackball_obj_t);
    Trackball::State state = self->breakout->read();

    mp_obj_t tuple[6];
    tuple[LEFT] = mp_obj_new_int(state.left);
    tuple[RIGHT] = mp_obj_new_int(state.right);
    tuple[UP] = mp_obj_new_int(state.up);
    tuple[DOWN] = mp_obj_new_int(state.down);
    tuple[SW_CHANGED] = mp_obj_new_int(state.sw_changed);
    tuple[SW_PRESSED] = mp_obj_new_int(state.sw_pressed);

    return mp_obj_new_tuple(6, tuple);
}
}