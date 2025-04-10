#include <cstdio>
#include "drivers/duo75/duo75.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "pico/multicore.h"

#include "micropython/modules/util.hpp"


using namespace pimoroni;

extern "C" {
#include "duo75.h"
#include "py/builtin.h"
#include "py/mpthread.h"
#include "micropython/modules/pimoroni_i2c/pimoroni_i2c.h"

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    void *fontdata;
    _PimoroniI2C_obj_t *i2c;
    bool blocking = true;
    uint8_t layers;
} ModPicoGraphics_obj_t;

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};
const uint DUO75_WIDTH = 128;
const uint DUO75_HEIGHT = 128;

/********** WS2812 **********/

/***** Variables Struct *****/
typedef struct _Duo75_obj_t {
    mp_obj_base_t base;
    Duo75* duo75;
    void *buf;
} _Duo75_obj_t;

_Duo75_obj_t *duo75_obj;


void __isr dma_complete() {
    if(duo75_obj) duo75_obj->duo75->dma_complete();
}

/***** Print *****/
void Duo75_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    (void)kind;
    mp_print_str(print, "Duo75(128x128)");
}

/***** Destructor ******/
mp_obj_t Duo75___del__(mp_obj_t self_in) {
    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Duo75_obj_t);
    self->duo75->stop(dma_complete);
    m_del_class(Duo75, self->duo75);
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t Duo75_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { 
        ARG_buffer,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    Pixel *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (Pixel *)bufinfo.buf;
        if(bufinfo.len < (size_t)(DUO75_WIDTH * DUO75_HEIGHT * sizeof(Pixel))) {
            mp_raise_ValueError(MP_ERROR_TEXT("Supplied buffer is too small!"));
        }
    } else {
        buffer = m_new(Pixel, DUO75_WIDTH * DUO75_HEIGHT);
    }

    duo75_obj = mp_obj_malloc_with_finaliser(_Duo75_obj_t, &Duo75_type);
    duo75_obj->buf = buffer;
    duo75_obj->duo75 = m_new_class(Duo75, buffer);

    return MP_OBJ_FROM_PTR(duo75_obj);
}

mp_obj_t Duo75_clear(mp_obj_t self_in) {
    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Duo75_obj_t);
    self->duo75->clear();
    return mp_const_none;
}

mp_obj_t Duo75_update(mp_obj_t self_in, mp_obj_t graphics_in) {
    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Duo75_obj_t);
    ModPicoGraphics_obj_t *picographics = MP_OBJ_TO_PTR2(graphics_in, ModPicoGraphics_obj_t);

    self->duo75->update(picographics->graphics);

    return mp_const_none;
}

mp_obj_t Duo75_start(mp_obj_t self_in) {
    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Duo75_obj_t);
    self->duo75->start(dma_complete);
    return mp_const_none;
}

mp_obj_t Duo75_stop(mp_obj_t self_in) {
    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Duo75_obj_t);
    self->duo75->stop(dma_complete);
    return mp_const_none;
}

mp_obj_t Duo75_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    _Duo75_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Duo75_obj_t);
    self->duo75->set_pixel(x, y, r, g, b);

    return mp_const_none;
}

}