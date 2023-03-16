#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_unicorn/pico_unicorn.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;


extern "C" {
#include "pico_unicorn.h"
#include "micropython/modules/pimoroni_i2c/pimoroni_i2c.h"
#include "py/builtin.h"

typedef struct _picounicorn_obj_t {
    mp_obj_base_t base;
    PicoUnicorn *unicorn;
} picounicorn_obj_t;

// from picographics/picographics.cpp
// used to support accepting a PicoGraphics class
typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    _PimoroniI2C_obj_t *i2c;
} ModPicoGraphics_obj_t;

mp_obj_t picounicorn_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) { 
    picounicorn_obj_t *self = m_new_obj_with_finaliser(picounicorn_obj_t);
    self->base.type = &picounicorn_type;
    self->unicorn = m_new_class(PicoUnicorn);
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t picounicorn__del__(mp_obj_t self_in) {
    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, picounicorn_obj_t);
    m_del_class(PicoUnicorn, self->unicorn);
    return mp_const_none;
}

mp_obj_t picounicorn_get_width(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoUnicorn::WIDTH);
}

mp_obj_t picounicorn_get_height(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoUnicorn::HEIGHT);
}

mp_obj_t picounicorn_set_pixel(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; // Unused input parameter, we know it's 5

    enum { ARG_self, ARG_x, ARG_y, ARG_r, ARG_g, ARG_b };

    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], picounicorn_obj_t);

    int x = mp_obj_get_int(args[ARG_x]);
    int y = mp_obj_get_int(args[ARG_y]);
    int r = mp_obj_get_int(args[ARG_r]);
    int g = mp_obj_get_int(args[ARG_g]);
    int b = mp_obj_get_int(args[ARG_b]);

    if(x < 0 || x >= PicoUnicorn::WIDTH || y < 0 || y >= PicoUnicorn::HEIGHT) {
        mp_raise_ValueError("x or y out of range.");
    }

    if(r < 0 || r > 255) mp_raise_ValueError("r out of range. Expected 0 to 255");
    if(g < 0 || g > 255) mp_raise_ValueError("g out of range. Expected 0 to 255");
    if(b < 0 || b > 255) mp_raise_ValueError("b out of range. Expected 0 to 255");
    self->unicorn->set_pixel(x, y, r, g, b);

    return mp_const_none;
}

mp_obj_t picounicorn_set_pixel_value(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_v };

    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], picounicorn_obj_t);

    int x = mp_obj_get_int(args[ARG_x]);
    int y = mp_obj_get_int(args[ARG_y]);
    int val = mp_obj_get_int(args[ARG_v]);

    if(x < 0 || x >= PicoUnicorn::WIDTH || y < 0 || y >= PicoUnicorn::HEIGHT) {
        mp_raise_ValueError("x or y out of range.");
    }

    if(val < 0 || val > 255) mp_raise_ValueError("val out of range. Expected 0 to 255");

    self->unicorn->set_pixel(x, y, val);

    return mp_const_none;
}

mp_obj_t picounicorn_clear(mp_obj_t self_in) {
    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, picounicorn_obj_t);
    self->unicorn->clear();
    return mp_const_none;
}

mp_obj_t picounicorn_is_pressed(mp_obj_t self_in, mp_obj_t button_obj) {
    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, picounicorn_obj_t);

    bool buttonPressed = false;
    int buttonID = mp_obj_get_int(button_obj);

    switch(buttonID) {
    case 0:
        buttonPressed = self->unicorn->is_pressed(PicoUnicorn::A);
        break;

    case 1:
        buttonPressed = self->unicorn->is_pressed(PicoUnicorn::B);
        break;

    case 2:
        buttonPressed = self->unicorn->is_pressed(PicoUnicorn::X);
        break;

    case 3:
        buttonPressed = self->unicorn->is_pressed(PicoUnicorn::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}

mp_obj_t picounicorn_update(mp_obj_t self_in, mp_obj_t graphics_in) {
    picounicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, picounicorn_obj_t);
    ModPicoGraphics_obj_t *picographics = MP_OBJ_TO_PTR2(graphics_in, ModPicoGraphics_obj_t);

    if(picographics->base.type == &ModPicoGraphics_type) {
        self->unicorn->update(picographics->graphics);
    }
    return mp_const_none;
}
}