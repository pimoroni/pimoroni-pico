#include "libraries/breakout_dotmatrix/breakout_dotmatrix.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>



using namespace pimoroni;

extern "C" {
#include "breakout_dotmatrix.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_dotmatrix_BreakoutDotMatrix_obj_t {
    mp_obj_base_t base;
    BreakoutDotMatrix *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_dotmatrix_BreakoutDotMatrix_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutDotMatrix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = BreakoutDotMatrix::DEFAULT_I2C_ADDRESS} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_dotmatrix_BreakoutDotMatrix_obj_t);
    self->base.type = &breakout_dotmatrix_BreakoutDotMatrix_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutDotMatrix, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_address].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "DotMatrix breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutDotMatrix_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness, ARG_update };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_update, MP_ARG_BOOL, {.u_bool = false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_dotmatrix_BreakoutDotMatrix_obj_t);

    float brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->breakout->set_brightness((uint8_t)(brightness * BreakoutDotMatrix::MAX_BRIGHTNESS), args[ARG_update].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_set_decimal(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_left, ARG_right };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_left, MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_right, MP_ARG_BOOL, { .u_bool = false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_dotmatrix_BreakoutDotMatrix_obj_t);
    self->breakout->set_decimal(args[ARG_left].u_bool, args[ARG_right].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_on };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_col, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_row, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_on, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_dotmatrix_BreakoutDotMatrix_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    if(x < 0 || x >= BreakoutDotMatrix::WIDTH || y < 0 || y >= BreakoutDotMatrix::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    else
        self->breakout->set_pixel(x, y, args[ARG_on].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_set_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_ch };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_col, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_dotmatrix_BreakoutDotMatrix_obj_t);

    int x = args[ARG_x].u_int;
    int ch = 0;

    if(mp_obj_is_str_or_bytes(args[ARG_ch].u_obj)) {
        GET_STR_DATA_LEN(args[ARG_ch].u_obj, str, str_len);
        if(str_len == 1) {
            ch = str[0];
        }
    } else {
        ch = mp_obj_get_int(args[ARG_ch].u_obj);
    }
    self->breakout->set_character(x, ch);

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_set_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_image, ARG_width, ARG_height, ARG_offset_x, ARG_offset_y, ARG_wrap, ARG_bg, ARG_on_level, ARG_padding };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_image, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_offset_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_offset_y, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_wr, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_bg, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_on_level, MP_ARG_INT, {.u_int = BreakoutDotMatrix::DEFAULT_ON_LEVEL} },
        { MP_QSTR_padding, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_dotmatrix_BreakoutDotMatrix_obj_t);

    int width = args[ARG_width].u_int;
    int height = args[ARG_height].u_int;
    int offset_x = args[ARG_offset_x].u_int;
    int offset_y = args[ARG_offset_y].u_int;
    int on_level = args[ARG_on_level].u_int;
    int padding = args[ARG_padding].u_int;

    if(width <= 0 || height <= 0)
        mp_raise_ValueError("width or height less than or equal to zero.");
    else if(offset_x < 0 || offset_y < 0)
        mp_raise_ValueError("offset_x or offset_y less than zero.");
    else if(on_level < 0 || offset_y > 255)
        mp_raise_ValueError("on_level out of range. Expected 0 to 255");
    else if(padding < 0 || padding > 255)
        mp_raise_ValueError("padding out of range. Expected 0 to 255");
    else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_image].u_obj, &bufinfo, MP_BUFFER_READ);

        self->breakout->set_image((const uint8_t *)bufinfo.buf, width, height,
                                  offset_x, offset_y, args[ARG_wrap].u_bool,
                                  args[ARG_bg].u_bool, on_level, padding);
    }

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_clear(mp_obj_t self_in) {
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_dotmatrix_BreakoutDotMatrix_obj_t);
    self->breakout->clear();

    return mp_const_none;
}

mp_obj_t BreakoutDotMatrix_show(mp_obj_t self_in) {
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_dotmatrix_BreakoutDotMatrix_obj_t);
    self->breakout->show();

    return mp_const_none;
}
}