#include "../../../libraries/breakout_dotmatrix/breakout_dotmatrix.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_dotmatrix.h"

/***** Variables Struct *****/
typedef struct _breakout_dotmatrix_BreakoutDotMatrix_obj_t {
    mp_obj_base_t base;
    BreakoutDotMatrix *breakout;
} breakout_dotmatrix_BreakoutDotMatrix_obj_t;

/***** Print *****/
void BreakoutDotMatrix_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_dotmatrix_BreakoutDotMatrix_obj_t);
    BreakoutDotMatrix* breakout = self->breakout;
    mp_print_str(print, "BreakoutDotMatrix(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutDotMatrix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_dotmatrix_BreakoutDotMatrix_obj_t *self = nullptr;

    if(n_args == 0) {
        mp_arg_check_num(n_args, n_kw, 0, 0, true);
        self = m_new_obj(breakout_dotmatrix_BreakoutDotMatrix_obj_t);
        self->base.type = &breakout_dotmatrix_BreakoutDotMatrix_type;
        self->breakout = new BreakoutDotMatrix();
    }
    else if(n_args == 1) {
        enum { ARG_address };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        self = m_new_obj(breakout_dotmatrix_BreakoutDotMatrix_obj_t);
        self->base.type = &breakout_dotmatrix_BreakoutDotMatrix_type;
        
        self->breakout = new BreakoutDotMatrix(args[ARG_address].u_int);
    }
    else {
        enum { ARG_i2c, ARG_address, ARG_sda, ARG_scl };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
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

        self = m_new_obj(breakout_dotmatrix_BreakoutDotMatrix_obj_t);
        self->base.type = &breakout_dotmatrix_BreakoutDotMatrix_type;
        
        i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
        self->breakout = new BreakoutDotMatrix(i2c, args[ARG_address].u_int, sda, scl);
    }

    self->breakout->init();

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
    int ch = mp_obj_get_int(args[ARG_ch].u_obj);
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