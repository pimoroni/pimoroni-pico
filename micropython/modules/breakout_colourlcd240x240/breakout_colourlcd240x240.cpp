#include "libraries/breakout_colourlcd240x240/breakout_colourlcd240x240.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

#define IS_VALID_PERIPH(spi, pin)   ((((pin) & 8) >> 3) == (spi))
#define IS_VALID_SCK(spi, pin)      (((pin) & 3) == 2 && IS_VALID_PERIPH(spi, pin))
#define IS_VALID_MOSI(spi, pin)     (((pin) & 3) == 3 && IS_VALID_PERIPH(spi, pin))
#define IS_VALID_MISO(spi, pin)     (((pin) & 3) == 0 && IS_VALID_PERIPH(spi, pin))


using namespace pimoroni;

extern "C" {
#include "breakout_colourlcd240x240.h"

/***** Variables Struct *****/
typedef struct _breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t {
    mp_obj_base_t base;
    BreakoutColourLCD240x240 *breakout;
} breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t;

/***** Print *****/
void BreakoutColourLCD240x240_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
    BreakoutColourLCD240x240* breakout = self->breakout;
    mp_print_str(print, "BreakoutColourLCD240x240(");

    mp_print_str(print, "spi = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_spi() == spi0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", cs = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_cs()), PRINT_REPR);

    mp_print_str(print, ", dc = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_dc()), PRINT_REPR);

    mp_print_str(print, ", sck = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sck()), PRINT_REPR);

    mp_print_str(print, ", mosi = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_mosi()), PRINT_REPR);

    mp_print_str(print, ", bl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_bl()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutColourLCD240x240_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = nullptr;

    if(n_args + n_kw == 2) {
        enum { ARG_buffer, ARG_slot };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_slot, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        int slot = args[ARG_slot].u_int;
        if(slot == ST7789::BG_SPI_FRONT || slot == ST7789::BG_SPI_BACK) {
            self = m_new_obj(breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
            self->base.type = &breakout_colourlcd240x240_BreakoutColourLCD240x240_type;

            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);

            self->breakout = new BreakoutColourLCD240x240((uint16_t *)bufinfo.buf, (ST7789::BG_SPI_SLOT)slot);
        }
        else {
            mp_raise_ValueError("slot not a valid value. Expected 0 to 1");
        }
    }
    else {
        enum { ARG_buffer, ARG_spi, ARG_cs, ARG_dc, ARG_sck, ARG_mosi, ARG_bl };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_spi, MP_ARG_INT, {.u_int = -1} },
            { MP_QSTR_cs, MP_ARG_INT, {.u_int = ST7789::DEFAULT_CS_PIN} },
            { MP_QSTR_dc, MP_ARG_INT, {.u_int = ST7789::DEFAULT_DC_PIN} },
            { MP_QSTR_sck, MP_ARG_INT, {.u_int = ST7789::DEFAULT_SCK_PIN} },
            { MP_QSTR_mosi, MP_ARG_INT, {.u_int = ST7789::DEFAULT_MOSI_PIN} },
            { MP_QSTR_bl, MP_ARG_INT, {.u_int = ST7789::DEFAULT_BL_PIN} },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);

        // Get SPI bus.
        int spi_id = args[ARG_spi].u_int;
        int sck = args[ARG_sck].u_int;
        int mosi = args[ARG_mosi].u_int;

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

        self = m_new_obj(breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
        self->base.type = &breakout_colourlcd240x240_BreakoutColourLCD240x240_type;

        spi_inst_t *spi = (spi_id == 0) ? spi0 : spi1;
        self->breakout = new BreakoutColourLCD240x240((uint16_t *)bufinfo.buf, spi,
            args[ARG_cs].u_int, args[ARG_dc].u_int, sck, mosi, BreakoutColourLCD240x240::PIN_UNUSED, args[ARG_bl].u_int);
    }

    self->breakout->init();

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutColourLCD240x240_update(mp_obj_t self_in) {
    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
    self->breakout->update();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    float brightness = mp_obj_get_float(args[ARG_brightness].u_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        self->breakout->set_backlight((uint8_t)(brightness * 255.0f));

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    if(n_args <= 2) {
        enum { ARG_self, ARG_pen };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_pen, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

        int pen = args[ARG_pen].u_int;

        if(pen < 0 || pen > 0xffff)
            mp_raise_ValueError("p is not a valid pen.");
        else
            self->breakout->set_pen(pen);
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

        breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

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
            self->breakout->set_pen(r, g, b);
    }

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

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
        pen = self->breakout->create_pen(r, g, b);

    return mp_obj_new_int(pen);
}

mp_obj_t BreakoutColourLCD240x240_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    Rect r(x, y, w, h);
    self->breakout->set_clip(r);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_remove_clip(mp_obj_t self_in) {
    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
    self->breakout->remove_clip();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_clear(mp_obj_t self_in) {
    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);
    self->breakout->clear();

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    Point p(x, y);
    self->breakout->pixel(p);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_l };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_l, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int l = args[ARG_l].u_int;

    Point p(x, y);
    self->breakout->pixel_span(p, l);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    Rect r(x, y, w, h);
    self->breakout->rectangle(r);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int r = args[ARG_r].u_int;

    Point p(x, y);
    self->breakout->circle(p, r);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int c = mp_obj_get_int(args[ARG_char].u_obj);
    int x = args[ARG_y].u_int;
    int y = args[ARG_y].u_int;

    Point p(x, y);
    if(n_args == 4) {
        int scale = args[ARG_scale].u_int;
        self->breakout->character((char)c, p, scale);
    }
    else
        self->breakout->character((char)c, p);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_x, ARG_y, ARG_wrap, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_wr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    mp_check_self(mp_obj_is_str_or_bytes(args[ARG_text].u_obj));
    GET_STR_DATA_LEN(args[ARG_text].u_obj, str, str_len);

    std::string t((const char*)str);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int wrap = args[ARG_wrap].u_int;

    Point p(x, y);
    if(n_args == 5) {
        int scale = args[ARG_scale].u_int;
        self->breakout->text(t, p, wrap, scale);
    }
    else
        self->breakout->text(t, p, wrap);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // enum { ARG_self, ARG_x, ARG_y, ARG_r };
    // static const mp_arg_t allowed_args[] = {
    //     { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
    //     { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
    //     { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
    //     { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
    // };

    // mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    // int x = args[ARG_x].u_int;
    // int y = args[ARG_y].u_int;
    // int r = args[ARG_r].u_int;

    // Point p(x, y);
    // self->breakout->circle(p, r);

    mp_raise_NotImplementedError("polygon is not implemented. Please avoid using this function for now");

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x1].u_int;
    int y2 = args[ARG_y1].u_int;
    int x3 = args[ARG_x1].u_int;
    int y3 = args[ARG_y1].u_int;

    Point p1(x1, y1);
    Point p2(x2, y2);
    Point p3(x3, y3);
    self->breakout->triangle(p1, p2, p3);

    return mp_const_none;
}

mp_obj_t BreakoutColourLCD240x240_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_colourlcd240x240_BreakoutColourLCD240x240_obj_t);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x1].u_int;
    int y2 = args[ARG_y1].u_int;

    Point p1(x1, y1);
    Point p2(x2, y2);
    self->breakout->line(p1, p2);

    return mp_const_none;
}
}