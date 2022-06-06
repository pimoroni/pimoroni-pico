#include <cstdio>
#include "hardware/watchdog.h"
#include "inky2040.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

extern "C" {
#include "inky2040.h"
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
typedef struct _Inky2040_obj_t {
    mp_obj_base_t base;
    pimoroni::Inky2040* inky2040;
    void *buf;
} _Inky2040_obj_t;

_Inky2040_obj_t *inky2040_obj;

/***** Print *****/
void Inky2040_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    (void)self_in;
    //_Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    mp_print_str(print, "Inky2040( ");
    mp_print_str(print, " )");
}

/***** Destructor ******/
mp_obj_t Inky2040___del__(mp_obj_t self_in) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    // Try to ensure power is cut off when soft reset (IE: "Stop" in Thonny)
    self->inky2040->power_off();
    delete self->inky2040;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t Inky2040_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int width = 600;
    int height = 448;

    uint8_t *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (uint8_t *)bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height / 2)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        buffer = m_new(uint8_t, width * height / 2);
    }

    inky2040_obj = m_new_obj_with_finaliser(_Inky2040_obj_t);
    inky2040_obj->base.type = &Inky2040_type;
    inky2040_obj->buf = buffer;
    inky2040_obj->inky2040 = new pimoroni::Inky2040(buffer);
    inky2040_obj->inky2040->init();

    return MP_OBJ_FROM_PTR(inky2040_obj);
}

mp_obj_t Inky2040_is_busy(mp_obj_t self_in) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    return self->inky2040->is_busy() ? mp_const_true : mp_const_false;
}

mp_obj_t Inky2040_update(mp_obj_t self_in) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);

    while(self->inky2040->is_busy()) {
#ifdef MICROPY_EVENT_POLL_HOOK
MICROPY_EVENT_POLL_HOOK
#endif
    }

    //absolute_time_t t_end = make_timeout_time_ms(26 * 100);
    self->inky2040->update(false);

    // Ensure blocking for the minimum amount of time
    // in cases where "is_busy" is unreliable.
    while(self->inky2040->is_busy() ) { //|| absolute_time_diff_us(t_end, get_absolute_time()) > 0) {
#ifdef MICROPY_EVENT_POLL_HOOK
MICROPY_EVENT_POLL_HOOK
#endif
    }

    self->inky2040->power_off();

    return mp_const_none;
}

mp_obj_t Inky2040_font(mp_obj_t self_in, mp_obj_t font) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    self->inky2040->font(mp_obj_to_string_r(font));
    return mp_const_none;
}

mp_obj_t Inky2040_pen(mp_obj_t self_in, mp_obj_t color) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    self->inky2040->pen(mp_obj_get_int(color));
    return mp_const_none;
}

mp_obj_t Inky2040_thickness(mp_obj_t self_in, mp_obj_t thickness) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    self->inky2040->thickness(mp_obj_get_int(thickness));
    return mp_const_none;
}

mp_obj_t Inky2040_clear(mp_obj_t self_in) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    self->inky2040->clear();
    return mp_const_none;
}

mp_obj_t Inky2040_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Inky2040_obj_t);
    self->inky2040->pixel(mp_obj_get_int(x), mp_obj_get_int(y));
    return mp_const_none;
}

mp_obj_t Inky2040_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->line(x1, y1, x2, y2);

    return mp_const_none;
}

mp_obj_t Inky2040_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->rectangle(x, y, w, h);

    return mp_const_none;
}

mp_obj_t Inky2040_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_data, ARG_w, ARG_h, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },

        { MP_QSTR_w, MP_ARG_INT, {.u_int = 600} },
        { MP_QSTR_h, MP_ARG_INT, {.u_int = 448} },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int dw = args[ARG_w].u_int;
    int dh = args[ARG_h].u_int;
    int dx = args[ARG_x].u_int;
    int dy = args[ARG_y].u_int;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_RW);
    if(bufinfo.len < (size_t)(dw * dh / 2)) {
        mp_raise_ValueError("image: Supplied buffer is too small!");
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->image((uint8_t *)bufinfo.buf, dw, dh, dx, dy);

    return mp_const_none;   
}

mp_obj_t Inky2040_icon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_data, ARG_icon_index, ARG_sheet_size, ARG_icon_size, ARG_dx, ARG_dy };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_icon_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sheet_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_icon_size, MP_ARG_INT, {.u_int = 64} },

        { MP_QSTR_dx, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_dy, MP_ARG_INT, {.u_int = 0} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int ssize = args[ARG_sheet_size].u_int;
    int isize = args[ARG_icon_size].u_int;
    int index = args[ARG_icon_index].u_int;
    int dx = args[ARG_dx].u_int;
    int dy = args[ARG_dy].u_int;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_RW);
        if(bufinfo.len < (size_t)(ssize * isize / 2)) {
        mp_raise_ValueError("icon: Supplied buffer is too small!");
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->icon((uint8_t *)bufinfo.buf, ssize, isize, index, dx, dy);

    return mp_const_none;   
}


mp_obj_t Inky2040_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_message, ARG_x, ARG_y, ARG_scale, ARG_rotation };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_message, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_rotation, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    std::string message = mp_obj_to_string_r(args[ARG_message].u_obj);
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    float scale = 1.0f;
    if (args[ARG_scale].u_obj != mp_const_none) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }
    float rotation = 0.0f;
    if (args[ARG_rotation].u_obj != mp_const_none) {
        rotation = mp_obj_get_float(args[ARG_rotation].u_obj);
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->text(message, x, y, scale, rotation);

    return mp_const_none;
}

mp_obj_t Inky2040_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_x, ARG_y, ARG_scale, ARG_rotation };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_rotation, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int c = args[ARG_char].u_int;
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    float scale = 1.0f;
    if (args[ARG_scale].u_obj != mp_const_none) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }
    float rotation = 0.0f;
    if (args[ARG_rotation].u_obj != mp_const_none) {
        rotation = mp_obj_get_float(args[ARG_rotation].u_obj);
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    self->inky2040->glyph(c, x, y, scale, rotation);

    return mp_const_none;
}

mp_obj_t Inky2040_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_message, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_message, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    std::string message = mp_obj_to_string_r(args[ARG_message].u_obj);
    float scale = 1.0f;
    if (args[ARG_scale].u_obj != mp_const_none) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    return mp_obj_new_int(self->inky2040->measure_text(message, scale));
}

mp_obj_t Inky2040_measure_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int c = args[ARG_char].u_int;
    float scale = 1.0f;
    if (args[ARG_scale].u_obj != mp_const_none) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }

    _Inky2040_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Inky2040_obj_t);
    return mp_obj_new_int(self->inky2040->measure_glyph(c, scale));
}

}
