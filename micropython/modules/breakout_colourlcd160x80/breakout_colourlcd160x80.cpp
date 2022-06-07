#include "libraries/breakout_colourlcd160x80/breakout_colourlcd160x80.hpp"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"

#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "breakout_colourlcd160x80.h"
#include "micropython/modules/pimoroni_bus/pimoroni_bus.h"

/***** Variables Struct *****/
typedef struct _BreakoutColourLCD160x80_obj_t {
    mp_obj_base_t base;
    BreakoutColourLCD160x80 *breakout;
    void *buffer;
} BreakoutColourLCD160x80_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutColourLCD160x80_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    BreakoutColourLCD160x80_obj_t *self = nullptr;

    enum { ARG_bus, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_buffer, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(BreakoutColourLCD160x80_obj_t);
    self->base.type = &BreakoutColourLCD160x80_type;

    // Get or create a suitable framebuffer
    // save a pointer onto the object so that GC can find it
    size_t required_size = PenRGB565::buffer_size(WIDTH, HEIGHT);

    if (args[ARG_buffer].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        self->buffer = bufinfo.buf;
        if(bufinfo.len < (size_t)(required_size)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        self->buffer = m_new(uint8_t, required_size);
    }

    // Check the bus argument for a valid SPIBus
    // or just build the class and accept the defaults.
    if (args[ARG_bus].u_obj == mp_const_none) {
        self->breakout = m_new_class(BreakoutColourLCD160x80, self->buffer);
    } else if (mp_obj_is_type(args[ARG_bus].u_obj, &ParallelPins_type)) {
        _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
        self->breakout = m_new_class(BreakoutColourLCD160x80, self->buffer, *(SPIPins *)(bus->pins));
    } else {
        mp_raise_ValueError("SPIBus expected!");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutColourLCD160x80_get_bounds(mp_obj_t self_in) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);
    mp_obj_t tuple[2] = {
        mp_obj_new_int(self->breakout->bounds.w),
        mp_obj_new_int(self->breakout->bounds.h)
    };
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t BreakoutColourLCD160x80_update(mp_obj_t self_in) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);
    self->breakout->update();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_set_backlight(mp_obj_t self_in, mp_obj_t brightness) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    float b = mp_obj_get_float(brightness);

    if(b < 0 || b > 1.0f) mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");

    self->breakout->set_backlight((uint8_t)(b * 255.0f));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_module_RGB332(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(PicoGraphicsPenType::rgb_to_rgb332(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ));
}

mp_obj_t BreakoutColourLCD160x80_module_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(PicoGraphicsPenType::rgb_to_rgb565(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ));
}

mp_obj_t BreakoutColourLCD160x80_set_pen(mp_obj_t self_in, mp_obj_t pen) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    self->breakout->set_pen(mp_obj_get_int(pen));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_reset_pen(mp_obj_t self_in, mp_obj_t pen) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    self->breakout->reset_pen(mp_obj_get_int(pen));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_update_pen(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_i, ARG_r, ARG_g, ARG_b };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->update_pen(
        mp_obj_get_int(args[ARG_i]) & 0xff,
        mp_obj_get_int(args[ARG_r]) & 0xff,
        mp_obj_get_int(args[ARG_g]) & 0xff,
        mp_obj_get_int(args[ARG_b]) & 0xff
    );

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_create_pen(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    int result = self->breakout->create_pen(
        mp_obj_get_int(args[ARG_r]) & 0xff,
        mp_obj_get_int(args[ARG_g]) & 0xff,
        mp_obj_get_int(args[ARG_b]) & 0xff
    );

    if (result == -1) mp_raise_ValueError("create_pen failed. No matching colour or space in palette!");

    return mp_obj_new_int(result);
}

mp_obj_t BreakoutColourLCD160x80_set_clip(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->set_clip({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y]),
        mp_obj_get_int(args[ARG_w]),
        mp_obj_get_int(args[ARG_h])
    });

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_remove_clip(mp_obj_t self_in) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    self->breakout->remove_clip();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_clear(mp_obj_t self_in) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    self->breakout->clear();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(self_in, BreakoutColourLCD160x80_obj_t);

    self->breakout->pixel({
        mp_obj_get_int(x),
        mp_obj_get_int(y)
    });

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_pixel_span(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_l };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->pixel_span({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y])
    },  mp_obj_get_int(args[ARG_l]));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_rectangle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->rectangle({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y]),
        mp_obj_get_int(args[ARG_w]),
        mp_obj_get_int(args[ARG_h])
    });

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_circle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->circle({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y])
    },  mp_obj_get_int(args[ARG_r]));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, BreakoutColourLCD160x80_obj_t);

    int c = mp_obj_get_int(args[ARG_char].u_obj);
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int scale = args[ARG_scale].u_int;

    self->breakout->character((char)c, Point(x, y), scale);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_x, ARG_y, ARG_wrap, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_wordwrap, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, BreakoutColourLCD160x80_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int wrap = args[ARG_wrap].u_int;
    int scale = args[ARG_scale].u_int;

    self->breakout->text(t, Point(x, y), wrap, scale);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, BreakoutColourLCD160x80_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    int scale = args[ARG_scale].u_int;

    int width = self->breakout->measure_text(t, scale);

    return mp_obj_new_int(width);
}

mp_obj_t BreakoutColourLCD160x80_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    size_t num_tuples = n_args - 1;
    const mp_obj_t *tuples = pos_args + 1;

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], BreakoutColourLCD160x80_obj_t);

    // Check if there is only one argument, which might be a list
    if(n_args == 2) {
        if(mp_obj_is_type(pos_args[1], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(pos_args[1], mp_obj_list_t);

            if(points->len <= 0) mp_raise_ValueError("poly(): cannot provide an empty list");

            num_tuples = points->len;
            tuples = points->items;
        }
        else {
            mp_raise_TypeError("poly(): can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        std::vector<Point> points;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) mp_raise_ValueError("poly(): can't convert object to tuple");

            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);

            if(tuple->len != 2) mp_raise_ValueError("poly(): tuple must only contain two numbers");

            points.push_back({
                mp_obj_get_int(tuple->items[0]),
                mp_obj_get_int(tuple->items[1])
            });
        }
        self->breakout->polygon(points);
    }

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_triangle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_x3, ARG_y3 };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->triangle(
        {mp_obj_get_int(args[ARG_x1]),
         mp_obj_get_int(args[ARG_y1])},
        {mp_obj_get_int(args[ARG_x2]),
         mp_obj_get_int(args[ARG_y2])},
        {mp_obj_get_int(args[ARG_x3]),
         mp_obj_get_int(args[ARG_y3])}
    );

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD160x80_line(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2 };

    BreakoutColourLCD160x80_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], BreakoutColourLCD160x80_obj_t);

    self->breakout->line(
        {mp_obj_get_int(args[ARG_x1]),
         mp_obj_get_int(args[ARG_y1])},
        {mp_obj_get_int(args[ARG_x2]),
         mp_obj_get_int(args[ARG_y2])}
    );

    return mp_const_none;
}
}
