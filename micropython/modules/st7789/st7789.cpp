#include "libraries/generic_st7789/generic_st7789.hpp"

#include "micropython/modules/util.hpp"



using namespace pimoroni;

extern "C" {
#include "st7789.h"

/***** Variables Struct *****/
typedef struct _GenericST7789_obj_t {
    mp_obj_base_t base;
    ST7789Generic *st7789;
    bool parallel;
    uint8_t *buffer;
} GenericST7789_obj_t;

/***** Print *****/
void GenericST7789_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);

    if(self->parallel) {
        mp_print_str(print, "ST7789Parallel()");
    } else {
        mp_print_str(print, "ST7789(");

        mp_print_str(print, "spi = ");
        mp_obj_print_helper(print, mp_obj_new_int((self->st7789->get_spi() == spi0) ? 0 : 1), PRINT_REPR);

        mp_print_str(print, ", cs = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->st7789->get_cs()), PRINT_REPR);

        mp_print_str(print, ", dc = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->st7789->get_dc()), PRINT_REPR);

        mp_print_str(print, ", sck = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->st7789->get_sck()), PRINT_REPR);

        mp_print_str(print, ", mosi = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->st7789->get_mosi()), PRINT_REPR);

        mp_print_str(print, ", bl = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->st7789->get_bl()), PRINT_REPR);

        mp_print_str(print, ")");
    }
}

/***** Constructor *****/
mp_obj_t GenericST7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    GenericST7789_obj_t *self = nullptr;

    enum { ARG_width, ARG_height, ARG_round, ARG_rotate180, ARG_slot, ARG_buffer, ARG_spi, ARG_cs, ARG_dc, ARG_sck, ARG_mosi, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_round, MP_ARG_OBJ, {.u_obj = mp_const_false} },
        { MP_QSTR_rotate180, MP_ARG_OBJ, {.u_obj = mp_const_false} },
        { MP_QSTR_slot, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_spi, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_cs, MP_ARG_INT, {.u_int = pimoroni::SPI_BG_FRONT_CS} },
        { MP_QSTR_dc, MP_ARG_INT, {.u_int = pimoroni::SPI_DEFAULT_MISO} },
        { MP_QSTR_sck, MP_ARG_INT, {.u_int = pimoroni::SPI_DEFAULT_SCK} },
        { MP_QSTR_mosi, MP_ARG_INT, {.u_int = pimoroni::SPI_DEFAULT_MOSI} },
        { MP_QSTR_bl, MP_ARG_INT, {.u_int = pimoroni::SPI_BG_FRONT_PWM} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(GenericST7789_obj_t);
    self->base.type = &GenericST7789_type;
    self->parallel = false;

    bool rotate180 = args[ARG_rotate180].u_obj == mp_const_true;
    bool round = args[ARG_round].u_obj == mp_const_true;
    int width = args[ARG_width].u_int;
    int height = args[ARG_height].u_int;

    if (args[ARG_buffer].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        self->buffer = (uint8_t *)bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        self->buffer = m_new(uint8_t, width * height);
    }

    if(args[ARG_slot].u_int != -1) {
        BG_SPI_SLOT slot = (BG_SPI_SLOT)args[ARG_slot].u_int;
        self->st7789 = m_new_class(ST7789Generic, width, height, round, self->buffer, slot);
        if (rotate180) {
            self->st7789->configure_display(true);
        }
    } else {
        // Get SPI bus.
        int spi_id = args[ARG_spi].u_int;
        int sck = args[ARG_sck].u_int;
        int mosi = args[ARG_mosi].u_int;
        int dc = args[ARG_dc].u_int;
        int cs = args[ARG_cs].u_int;
        int bl = args[ARG_bl].u_int;

        if(spi_id == -1) {
            spi_id = (sck >> 3) & 0b1;  // If no spi specified, choose the one for the given SCK pin
        }
        if(spi_id < 0 || spi_id > 1) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
        }

        if(!IS_VALID_SCK(spi_id, sck)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCK pin"));
        }

        if(!IS_VALID_MOSI(spi_id, mosi)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MOSI pin"));
        }
        spi_inst_t *spi = (spi_id == 0) ? spi0 : spi1;
        self->st7789 = m_new_class(ST7789Generic, width, height, round, self->buffer,
            spi, cs, dc, sck, mosi, bl);
        if (rotate180) {
            self->st7789->configure_display(true);
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t GenericST7789Parallel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    GenericST7789_obj_t *self = nullptr;

    enum { ARG_width, ARG_height, ARG_cs, ARG_dc, ARG_wr_sck, ARG_rd_sck, ARG_d0, ARG_bl, ARG_rotate180, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_cs, MP_ARG_INT, {.u_int = 10} },
        { MP_QSTR_dc, MP_ARG_INT, {.u_int = 11} },
        { MP_QSTR_wr_sck, MP_ARG_INT, {.u_int = 12} },
        { MP_QSTR_rd_sck, MP_ARG_INT, {.u_int = 13} },
        { MP_QSTR_d0, MP_ARG_INT, {.u_int = 14} },

        { MP_QSTR_bl, MP_ARG_INT, {.u_int = 2} },
        { MP_QSTR_rotate180, MP_ARG_OBJ, {.u_obj = mp_const_false} },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(GenericST7789_obj_t);
    self->base.type = &GenericST7789_type;
    self->parallel = true;

    bool rotate180 = args[ARG_rotate180].u_obj == mp_const_true;
    int width = args[ARG_width].u_int;
    int height = args[ARG_height].u_int;

    if (args[ARG_buffer].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        self->buffer = (uint8_t *)bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        self->buffer = m_new(uint8_t, width * height);
    }

    int cs = args[ARG_cs].u_int;
    int dc = args[ARG_dc].u_int;
    int wr_sck = args[ARG_wr_sck].u_int;
    int rd_sck = args[ARG_rd_sck].u_int;
    int d0 = args[ARG_d0].u_int;
    int bl = args[ARG_bl].u_int;

    self->st7789 = m_new_class(ST7789Generic, width, height, self->buffer,
        cs, dc, wr_sck, rd_sck, d0, bl);
    if (rotate180) {
        self->st7789->configure_display(true);
    }

    mp_printf(&mp_plat_print, "ST7789Generic - %lu\n", sizeof(ST7789Generic));

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t GenericST7789_update(mp_obj_t self_in) {
    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(self_in, GenericST7789_obj_t);
    self->st7789->update();

    return mp_const_none;
}

mp_obj_t GenericST7789_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    float brightness = mp_obj_get_float(args[ARG_brightness].u_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->st7789->set_backlight((uint8_t)(brightness * 255.0f));

    return mp_const_none;
}

mp_obj_t GenericST7789_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    if(n_args <= 2) {
        enum { ARG_self, ARG_pen, ARG_raw };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_pen, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_raw, MP_ARG_OBJ, { .u_obj = mp_const_false } },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

        int pen = args[ARG_pen].u_int;

        if (args[ARG_raw].u_obj == mp_const_false) {
            if(pen < 0 || pen > 0xff) {
                mp_raise_ValueError("p is not a valid pen.");
            } else {
                self->st7789->set_pen(pen);
            }
        } else {
            if(pen < 0 || pen > 0xffff) {
                mp_raise_ValueError("p is not a valid pen.");
            } else {
                self->st7789->set_pen_raw(pen);
            }
        }
    }
    else {
        enum { ARG_self, ARG_r, ARG_g, ARG_b };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

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
            self->st7789->set_pen(r, g, b);
    }

    return mp_const_none;
}

mp_obj_t GenericST7789_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    int pen = 0;

    enum { ARG_self, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

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
        pen = self->st7789->create_pen(r, g, b);

    return mp_obj_new_int(pen);
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

mp_obj_t GenericST7789_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    GenericST7789_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, GenericST7789_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    Point p(x, y);
    self->st7789->pixel(p);

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
    if(mp_obj_is_str_or_bytes(text_obj)) {
        GET_STR_DATA_LEN(text_obj, str, str_len);

        std::string t((const char*)str);

        int x = args[ARG_x].u_int;
        int y = args[ARG_y].u_int;
        int wrap = args[ARG_wrap].u_int;
        int scale = args[ARG_scale].u_int;

        self->st7789->text(t, Point(x, y), wrap, scale);
    }
    else if(mp_obj_is_float(text_obj)) {
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    }
    else if(mp_obj_is_int(text_obj)) {
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    }
    else if(mp_obj_is_bool(text_obj)) {
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    }
    else {
        mp_raise_TypeError("can't convert object to str implicitly");
    }

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
