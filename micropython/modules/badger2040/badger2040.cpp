#include <cstdio>
#include "badger2040.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))


extern "C" {
#include "badger2040.h"
#include "py/builtin.h"
#include "py/mpthread.h"

std::string mp_obj_to_string_r(const mp_obj_t &obj) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        return (const char*)str;
    }
    else if(mp_obj_is_float(obj))
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    else if(mp_obj_is_int(obj))
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    else if(mp_obj_is_bool(obj))
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    else
        mp_raise_TypeError("can't convert object to str implicitly");
}

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

/********** WS2812 **********/

/***** Variables Struct *****/
typedef struct _Badger2040_obj_t {
    mp_obj_base_t base;
    pimoroni::Badger2040* badger2040;
    void *buf;
} _Badger2040_obj_t;

_Badger2040_obj_t *badger2040_obj;

/***** Print *****/
void Badger2040_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    (void)self_in;
    //_Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    mp_print_str(print, "Badger2040( ");
    mp_print_str(print, " )");
}

/***** Destructor ******/
mp_obj_t Badger2040___del__(mp_obj_t self_in) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    delete self->badger2040;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t Badger2040_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int width = 296;
    int height = 128;

    uint8_t *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (uint8_t *)bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height / 8)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        buffer = m_new(uint8_t, width * height / 8);
    }

    badger2040_obj = m_new_obj_with_finaliser(_Badger2040_obj_t);
    badger2040_obj->base.type = &Badger2040_type;
    badger2040_obj->buf = buffer;
    badger2040_obj->badger2040 = new pimoroni::Badger2040(buffer);
    badger2040_obj->badger2040->init();

    return MP_OBJ_FROM_PTR(badger2040_obj);
}

mp_obj_t Badger2040_update(mp_obj_t self_in) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);

    self->badger2040->update();

    return mp_const_none;
}

mp_obj_t Badger2040_partial_update(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_x].u_int;
    int h = args[ARG_y].u_int;

    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Badger2040_obj_t);
    self->badger2040->partial_update(x, y, w, h);

    return mp_const_none;
}

// halt
// sleep

mp_obj_t Badger2040_led(mp_obj_t self_in, mp_obj_t brightness) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->pen(mp_obj_get_int(brightness));
    return mp_const_none;
}

mp_obj_t Badger2040_font(mp_obj_t self_in, mp_obj_t font) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->font(mp_obj_to_string_r(font));
    return mp_const_none;
}

mp_obj_t Badger2040_pen(mp_obj_t self_in, mp_obj_t color) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->pen(mp_obj_get_int(color));
    return mp_const_none;
}

mp_obj_t Badger2040_thickness(mp_obj_t self_in, mp_obj_t thickness) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->thickness(mp_obj_get_int(thickness));
    return mp_const_none;
}

mp_obj_t Badger2040_pressed(mp_obj_t self_in, mp_obj_t button) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->update_button_states();
    bool state = self->badger2040->pressed(mp_obj_get_int(button));
    return state ? mp_const_true : mp_const_false;
}

// pressed
// pressed_to_wake
// wait_for_press - implement in terms of MicroPython!
// update_button_states
// button_states

mp_obj_t Badger2040_clear(mp_obj_t self_in) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->clear();
    return mp_const_none;
}

mp_obj_t Badger2040_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Badger2040_obj_t);
    self->badger2040->pixel(mp_obj_get_int(x), mp_obj_get_int(y));
    return mp_const_none;
}

mp_obj_t Badger2040_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r2, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x2].u_int;
    int y2 = args[ARG_y2].u_int;

    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Badger2040_obj_t);
    self->badger2040->line(x1, y1, x2, y2);

    return mp_const_none;
}

mp_obj_t Badger2040_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[ARG_x1].u_int;
    int y = args[ARG_y1].u_int;
    int w = args[ARG_x2].u_int;
    int h = args[ARG_y2].u_int;

    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Badger2040_obj_t);
    self->badger2040->rectangle(x, y, w, h);

    return mp_const_none;
}

// image

mp_obj_t Badger2040_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_REQUIRED | MP_ARG_OBJ }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int c = args[ARG_char].u_int;
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    float scale = mp_obj_get_float(args[ARG_scale].u_obj);

    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Badger2040_obj_t);
    self->badger2040->glyph(c, x, y, scale);

    return mp_const_none;
}

mp_obj_t Badger2040_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_message, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_message, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_REQUIRED | MP_ARG_OBJ }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    std::string message = mp_obj_to_string_r(args[ARG_message].u_obj);
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    float scale = mp_obj_get_float(args[ARG_scale].u_obj);

    _Badger2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Badger2040_obj_t);
    self->badger2040->text(message, x, y, scale);

    return mp_const_none;
}

}