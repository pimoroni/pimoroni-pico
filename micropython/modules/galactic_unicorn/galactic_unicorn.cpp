#include "libraries/galactic_unicorn/galactic_unicorn.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>
#include <cfloat>


using namespace pimoroni;

extern "C" {
#include "galactic_unicorn.h"
#include "micropython/modules/pimoroni_i2c/pimoroni_i2c.h"
#include "py/builtin.h"


/********** GalacticUnicorn **********/

/***** Variables Struct *****/
typedef struct _GalacticUnicorn_obj_t {
    mp_obj_base_t base;
    GalacticUnicorn* galactic;
} _GalacticUnicorn_obj_t;

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    _PimoroniI2C_obj_t *i2c;
    //mp_obj_t scanline_callback; // Not really feasible in MicroPython
} ModPicoGraphics_obj_t;

/***** Print *****/
void GalacticUnicorn_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    //_GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    mp_print_str(print, "GalacticUnicorn()");
}


/***** Constructor *****/
mp_obj_t GalacticUnicorn_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _GalacticUnicorn_obj_t *self = nullptr;

    enum { ARG_pio, ARG_sm, ARG_pins, ARG_common_pin, ARG_direction, ARG_counts_per_rev, ARG_count_microsteps, ARG_freq_divider };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pio, MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pio_int = args[ARG_pio].u_int;
    if(pio_int < 0 || pio_int > (int)NUM_PIOS) {
        mp_raise_ValueError("pio out of range. Expected 0 to 1");
    }
    //PIO pio = pio_int == 0 ? pio0 : pio1;

    int sm = args[ARG_sm].u_int;
    if(sm < 0 || sm > (int)NUM_PIO_STATE_MACHINES) {
        mp_raise_ValueError("sm out of range. Expected 0 to 3");
    }


    GalacticUnicorn *galactic = m_new_class(GalacticUnicorn);
    galactic->init();

    self = m_new_obj_with_finaliser(_GalacticUnicorn_obj_t);
    self->base.type = &GalacticUnicorn_type;
    self->galactic = galactic;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t GalacticUnicorn___del__(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    m_del_class(GalacticUnicorn, self->galactic);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t GalacticUnicorn_clear(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->clear();
    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_update(mp_obj_t self_in, mp_obj_t graphics_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    ModPicoGraphics_obj_t *picographics = MP_OBJ_TO_PTR2(graphics_in, ModPicoGraphics_obj_t);

    self->galactic->update(picographics->graphics);

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_set_brightness(mp_obj_t self_in, mp_obj_t value) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->set_brightness(mp_obj_get_float(value));
    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_get_brightness(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    return mp_obj_new_float(self->galactic->get_brightness());
}

extern mp_obj_t GalacticUnicorn_adjust_brightness(mp_obj_t self_in, mp_obj_t delta) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->adjust_brightness(mp_obj_get_float(delta));
    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_set_volume(mp_obj_t self_in, mp_obj_t value) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->set_volume(mp_obj_get_float(value));
    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_get_volume(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    return mp_obj_new_float(self->galactic->get_volume());
}

extern mp_obj_t GalacticUnicorn_adjust_volume(mp_obj_t self_in, mp_obj_t delta) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->adjust_volume(mp_obj_get_float(delta));
    return mp_const_none;
}


extern mp_obj_t GalacticUnicorn_light(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    return mp_obj_new_float(self->galactic->light());
}

extern mp_obj_t GalacticUnicorn_is_pressed(mp_obj_t self_in, mp_obj_t button) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    return mp_obj_new_bool(self->galactic->is_pressed((uint8_t)mp_obj_get_int(button)));
}

extern mp_obj_t GalacticUnicorn_play_sample(mp_obj_t self_in, mp_obj_t data) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_RW);
    if(bufinfo.len < 1) {
        mp_raise_ValueError("Supplied buffer is too small!");
    }

    self->galactic->play_sample((uint8_t *)bufinfo.buf, bufinfo.len);

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_play_tone(mp_obj_t self_in, mp_obj_t freq) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->play_tone(mp_obj_get_float(freq));

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_play_dual_tone(mp_obj_t self_in, mp_obj_t freq_a, mp_obj_t freq_b) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->play_dual_tone(mp_obj_get_float(freq_a), mp_obj_get_float(freq_b));

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_stop_playing(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->stop_playing();

    return mp_const_none;
}
}
