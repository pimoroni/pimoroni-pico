#include "drivers/plasma/ws2812.hpp"
#include "drivers/plasma/apa102.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace plasma;

extern "C" {
#include "plasma.h"
#include "py/builtin.h"

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

/********** WS2812 **********/

/***** Variables Struct *****/
typedef struct _PlasmaWS2812_obj_t {
    mp_obj_base_t base;
    WS2812* ws2812;
    void *buf;
} _PlasmaWS2812_obj_t;


/***** Print *****/
void PlasmaWS2812_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaWS2812_obj_t);
    mp_print_str(print, "WS2812(");

    mp_print_str(print, "num_leds = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->ws2812->num_leds), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Destructor ******/
mp_obj_t PlasmaWS2812___del__(mp_obj_t self_in) {
    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaWS2812_obj_t);
    delete self->ws2812;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t PlasmaWS2812_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PlasmaWS2812_obj_t *self = nullptr;

    enum { 
        ARG_num_leds,
        ARG_pio,
        ARG_sm,
        ARG_dat,
        ARG_freq,
        ARG_buffer,
        ARG_rgbw,
        ARG_color_order
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_num_leds, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_dat, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = WS2812::DEFAULT_SERIAL_FREQ} },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_rgbw, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_color_order, MP_ARG_INT, {.u_int = (uint8_t)WS2812::COLOR_ORDER::GRB} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int num_leds = args[ARG_num_leds].u_int;
    PIO pio = args[ARG_pio].u_int == 0 ? pio0 : pio1;
    int sm = args[ARG_sm].u_int;
    int dat = args[ARG_dat].u_int;
    int freq = args[ARG_freq].u_int;
    bool rgbw = args[ARG_rgbw].u_bool;
    WS2812::COLOR_ORDER color_order = (WS2812::COLOR_ORDER)args[ARG_color_order].u_int;

    void *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = bufinfo.buf;
        if(bufinfo.len < (size_t)(num_leds * 4)) {
            mp_raise_ValueError("Supplied buffer is too small for LED count!");
        }
    }

    self = m_new_obj_with_finaliser(_PlasmaWS2812_obj_t);
    self->base.type = &PlasmaWS2812_type;
    self->buf = buffer;

    self->ws2812 = new WS2812(num_leds, pio, sm, dat, freq, rgbw, color_order, (WS2812::RGB *)buffer);

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t PlasmaWS2812_clear(mp_obj_t self_in) {
    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaWS2812_obj_t);
    self->ws2812->clear();
    return mp_const_none;
}

mp_obj_t PlasmaWS2812_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_fps };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_fps, MP_ARG_INT, {.u_int = 60} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaWS2812_obj_t);
    self->ws2812->start(args[ARG_fps].u_int);

    return mp_const_none;
}


mp_obj_t PlasmaWS2812_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_r, ARG_g, ARG_b, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_INT, {.u_int = 0} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;
    int w = args[ARG_w].u_int;

    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaWS2812_obj_t);

    self->ws2812->set_rgb(index, r, g, b, w);

    return mp_const_none;
}

mp_obj_t PlasmaWS2812_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_h, ARG_s, ARG_v, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hue, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sat, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} },
        { MP_QSTR_val, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} },
        { MP_QSTR_w, MP_ARG_INT, {.u_int = 0} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    float h = mp_obj_get_float(args[ARG_h].u_obj);
    float s = mp_obj_get_float(args[ARG_s].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);
    int w = args[ARG_w].u_int;

    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaWS2812_obj_t);

    self->ws2812->set_hsv(index, h, s, v, w);

    return mp_const_none;
}

mp_obj_t PlasmaWS2812_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;

    _PlasmaWS2812_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaWS2812_obj_t);
    WS2812::RGB rgb = self->ws2812->get(index);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(rgb.r);
    tuple[1] = mp_obj_new_float(rgb.g);
    tuple[2] = mp_obj_new_float(rgb.b);
    tuple[3] = mp_obj_new_float(rgb.w);
    return mp_obj_new_tuple(4, tuple);
}

/********** APA102 **********/

/***** Variables Struct *****/
typedef struct _PlasmaAPA102_obj_t {
    mp_obj_base_t base;
    APA102* apa102;
    void *buf;
} _PlasmaAPA102_obj_t;


/***** Print *****/
void PlasmaAPA102_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaAPA102_obj_t);
    mp_print_str(print, "APA102(");

    mp_print_str(print, "num_leds = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->apa102->num_leds), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Destructor ******/
mp_obj_t PlasmaAPA102___del__(mp_obj_t self_in) {
    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaAPA102_obj_t);
    delete self->apa102;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t PlasmaAPA102_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PlasmaAPA102_obj_t *self = nullptr;

    enum { 
        ARG_num_leds,
        ARG_pio,
        ARG_sm,
        ARG_dat,
        ARG_clk,
        ARG_freq,
        ARG_buffer
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_num_leds, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_dat, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_clk, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = APA102::DEFAULT_SERIAL_FREQ} },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int num_leds = args[ARG_num_leds].u_int;
    PIO pio = args[ARG_pio].u_int == 0 ? pio0 : pio1;
    int sm = args[ARG_sm].u_int;
    int dat = args[ARG_dat].u_int;
    int clk = args[ARG_clk].u_int;
    int freq = args[ARG_freq].u_int;

    APA102::RGB *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (APA102::RGB *)bufinfo.buf;
        if(bufinfo.len < (size_t)(num_leds * 4)) {
            mp_raise_ValueError("Supplied buffer is too small for LED count!");
        }
        // If a bytearray is supplied it'll be raw, uninitialized bytes
        // iterate through the RGB elements and call "brightness"
        // to set up the SOF bytes, otherwise a flickery mess will happen!
        // Oh for such niceties as "placement new"...
        for(auto i = 0; i < num_leds; i++) {
            buffer[i].brightness(15);
        }
    }

    self = m_new_obj_with_finaliser(_PlasmaAPA102_obj_t);
    self->base.type = &PlasmaAPA102_type;
    self->buf = buffer;

    self->apa102 = new APA102(num_leds, pio, sm, dat, clk, freq, buffer);

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t PlasmaAPA102_clear(mp_obj_t self_in) {
    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PlasmaAPA102_obj_t);
    self->apa102->clear();
    return mp_const_none;
}

mp_obj_t PlasmaAPA102_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_fps };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_fps, MP_ARG_INT, {.u_int = 60} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaAPA102_obj_t);
    self->apa102->start(args[ARG_fps].u_int);

    return mp_const_none;
}

mp_obj_t PlasmaAPA102_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int brightness = args[ARG_brightness].u_int;

    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaAPA102_obj_t);
    self->apa102->set_brightness(brightness);

    return mp_const_none;
}

mp_obj_t PlasmaAPA102_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaAPA102_obj_t);
    self->apa102->set_rgb(index, r, g, b);

    return mp_const_none;
}

mp_obj_t PlasmaAPA102_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_h, ARG_s, ARG_v };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hue, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sat, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} },
        { MP_QSTR_val, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    float h = mp_obj_get_float(args[ARG_h].u_obj);
    float s = mp_obj_get_float(args[ARG_s].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);

    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaAPA102_obj_t);
    self->apa102->set_hsv(index, h, s, v);

    return mp_const_none;
}

mp_obj_t PlasmaAPA102_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;

    _PlasmaAPA102_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PlasmaAPA102_obj_t);
    APA102::RGB rgb = self->apa102->get(index);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(rgb.r);
    tuple[1] = mp_obj_new_float(rgb.g);
    tuple[2] = mp_obj_new_float(rgb.b);
    tuple[3] = mp_obj_new_float(rgb.sof);
    return mp_obj_new_tuple(4, tuple);
}

}