#include "libraries/picographics_st7789/picographics_st7789.hpp"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"

#include "micropython/modules/util.hpp"



using namespace pimoroni;

extern "C" {
#include "st7789.h"
#include "micropython/modules/pimoroni_bus/pimoroni_bus.h"

enum ST7789Display {
    DISPLAY_LCD_240X240=0,
    DISPLAY_ROUND_LCD_240X240,
    DISPLAY_PICO_DISPLAY,
    DISPLAY_PICO_DISPLAY_2,
    DISPLAY_PICO_EXPLORER,
    DISPLAY_TUFTY_2040,
    DISPLAY_ENVIRO_PLUS
};

typedef struct _GenericST7789_obj_t {
    mp_obj_base_t base;
    PicoGraphicsST7789 *st7789;
    void *buffer;
} GenericST7789_obj_t;

mp_obj_t GenericST7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    GenericST7789_obj_t *self = nullptr;

    enum { ARG_display, ARG_rotate, ARG_bus, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_rotate, MP_ARG_INT, { .u_int = Rotation::ROTATE_0 } },
        { MP_QSTR_bus, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_buffer, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(GenericST7789_obj_t);
    self->base.type = &GenericST7789_type;

    Rotation rotate = (Rotation)args[ARG_rotate].u_int;
    bool round = false;
    int width = 0;
    int height = 0;

    ST7789Display display = (ST7789Display)args[ARG_display].u_int;

    switch(display) {
        case DISPLAY_PICO_DISPLAY:
            width = 240;
            height = 135;
            break;
        case DISPLAY_PICO_DISPLAY_2:
        case DISPLAY_TUFTY_2040:
            width = 320;
            height = 240;
            break;
        case DISPLAY_PICO_EXPLORER:
        case DISPLAY_LCD_240X240:
        case DISPLAY_ENVIRO_PLUS:
            width = 240;
            height = 240;
            break;
        case DISPLAY_ROUND_LCD_240X240:
            width = 240;
            height = 240;
            round = true;
            break;
    }

    if (args[ARG_buffer].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        self->buffer = bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height * sizeof(Pen))) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        self->buffer = m_new(uint8_t, width * height * sizeof(Pen));
    }

    if (display == DISPLAY_TUFTY_2040) {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->st7789 = m_new_class(PicoGraphicsST7789, width, height, rotate, self->buffer, {10, 11, 12, 13, 14, 2});
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &ParallelPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->st7789 = m_new_class(PicoGraphicsST7789, width, height, rotate, self->buffer, *(ParallelPins *)(bus->pins));
        } else {
            mp_raise_ValueError("ParallelBus expected!");
        }
    } else {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->st7789 = m_new_class(PicoGraphicsST7789, width, height, rotate, round, self->buffer, get_spi_pins(BG_SPI_FRONT));
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->st7789 = m_new_class(PicoGraphicsST7789, width, height, rotate, round, self->buffer, *(SPIPins *)(bus->pins));
        } else {
            mp_raise_ValueError("SPIBus expected!");
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t GenericST7789_set_framebuffer(mp_obj_t self_in, mp_obj_t framebuffer) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    if (framebuffer == mp_const_none) {
        m_del(uint8_t, self->buffer, self->st7789->bounds.w * self->st7789->bounds.h);
        self->buffer = nullptr;
        self->st7789->set_framebuffer(nullptr);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(framebuffer, &bufinfo, MP_BUFFER_RW);
        self->buffer = bufinfo.buf;
        self->st7789->set_framebuffer(self->buffer);
    }

    return mp_const_none;
}

mp_obj_t GenericST7789_get_bounds(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int(self->st7789->bounds.w);
    tuple[1] = mp_obj_new_int(self->st7789->bounds.h);
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t GenericST7789_update(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);
    self->st7789->update();

    return mp_const_none;
}

mp_obj_t GenericST7789_set_backlight(mp_obj_t self_in, mp_obj_t brightness) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    float b = mp_obj_get_float(brightness);

    if(b < 0 || b > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->st7789->set_backlight((uint8_t)(b * 255.0f));

    return mp_const_none;
}

mp_obj_t GenericST7789_module_RGB332(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(PicoGraphicsST7789::create_pen_rgb332(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ));
}

mp_obj_t GenericST7789_module_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(PicoGraphicsST7789::create_pen_rgb565(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ));
}

mp_obj_t GenericST7789_set_pen(mp_obj_t self_in, mp_obj_t pen) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    self->st7789->set_pen(mp_obj_get_int(pen) & 0xff);

    return mp_const_none;
}

mp_obj_t GenericST7789_set_palette_mode(mp_obj_t self_in, mp_obj_t mode) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    self->st7789->set_palette_mode((PicoGraphicsST7789::PaletteMode)mp_obj_get_int(mode));

    return mp_const_none;
}

mp_obj_t GenericST7789_set_palette(mp_obj_t self_in, mp_obj_t index, mp_obj_t colour) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    self->st7789->set_palette(
        mp_obj_get_int(index) & 0xff,
        mp_obj_get_int(colour) & 0xffff
    );

    return mp_const_none;
}

mp_obj_t GenericST7789_reserve_palette(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    int result = self->st7789->reserve_palette();

    if (result == -1) {
        mp_raise_ValueError("reserve_palette failed. No space in palette!");
    }

    return mp_obj_new_int(result);
}

mp_obj_t GenericST7789_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int result = self->st7789->create_pen(
        args[ARG_r].u_int & 0xff,
        args[ARG_g].u_int & 0xff,
        args[ARG_b].u_int & 0xff
    );

    if (result == -1) {
        mp_raise_ValueError("create_pen failed. No matching colour or space in palette!");
    }

    return mp_obj_new_int(result);
}

mp_obj_t GenericST7789_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    Rect r(x, y, w, h);
    self->st7789->set_clip(r);

    return mp_const_none;
}

mp_obj_t GenericST7789_remove_clip(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);
    self->st7789->remove_clip();

    return mp_const_none;
}

mp_obj_t GenericST7789_clear(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);
    self->st7789->clear();

    return mp_const_none;
}

mp_obj_t GenericST7789_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    self->st7789->pixel(Point(
        mp_obj_get_int(x),
        mp_obj_get_int(y)
    ));

    return mp_const_none;
}

mp_obj_t GenericST7789_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_l };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_l, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int l = args[ARG_l].u_int;

    Point p(x, y);
    self->st7789->pixel_span(p, l);

    return mp_const_none;
}

mp_obj_t GenericST7789_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    Rect r(x, y, w, h);
    self->st7789->rectangle(r);

    return mp_const_none;
}

mp_obj_t GenericST7789_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int r = args[ARG_r].u_int;

    Point p(x, y);
    self->st7789->circle(p, r);

    return mp_const_none;
}

mp_obj_t GenericST7789_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int c = mp_obj_get_int(args[ARG_char].u_obj);
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int scale = args[ARG_scale].u_int;

    self->st7789->character((char)c, Point(x, y), scale);

    return mp_const_none;
}

mp_obj_t GenericST7789_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int wrap = args[ARG_wrap].u_int;
    int scale = args[ARG_scale].u_int;

    self->st7789->text(t, Point(x, y), wrap, scale);

    return mp_const_none;
}

mp_obj_t GenericST7789_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    int scale = args[ARG_scale].u_int;

    int width = self->st7789->measure_text(t, scale);

    return mp_obj_new_int(width);

    return mp_const_none;
}

mp_obj_t GenericST7789_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    size_t num_tuples = n_args - 1;
    const mp_obj_t *tuples = pos_args + 1;

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], GenericST7789_obj_t);

    // Check if there is only one argument, which might be a list
    if(n_args == 2) {
        if(mp_obj_is_type(pos_args[1], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(pos_args[1], mp_obj_list_t);
            if(points->len > 0) {
                num_tuples = points->len;
                tuples = points->items;
            }
            else {
                mp_raise_ValueError("poly(): cannot provide an empty list");
            }
        }
        else {
            mp_raise_TypeError("poly(): can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        std::vector<Point> points;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                mp_raise_ValueError("poly(): can't convert object to tuple");
            }
            else {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                if(tuple->len != 2) {
                    mp_raise_ValueError("poly(): tuple must only contain two numbers");
                }
                points.push_back(Point(
                    mp_obj_get_int(tuple->items[0]),
                    mp_obj_get_int(tuple->items[1]))
                );
            }
        }
        self->st7789->polygon(points);
    }

    return mp_const_none;
}

mp_obj_t GenericST7789_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_x3, ARG_y3 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x3, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y3, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x2].u_int;
    int y2 = args[ARG_y2].u_int;
    int x3 = args[ARG_x3].u_int;
    int y3 = args[ARG_y3].u_int;

    Point p1(x1, y1);
    Point p2(x2, y2);
    Point p3(x3, y3);
    self->st7789->triangle(p1, p2, p3);

    return mp_const_none;
}

mp_obj_t GenericST7789_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x2].u_int;
    int y2 = args[ARG_y2].u_int;

    Point p1(x1, y1);
    Point p2(x2, y2);
    self->st7789->line(p1, p2);

    return mp_const_none;
}
}
