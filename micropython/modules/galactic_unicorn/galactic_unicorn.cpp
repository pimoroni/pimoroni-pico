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


/********** Channel **********/

/***** Variables Struct *****/
typedef struct _Channel_obj_t {
    mp_obj_base_t base;
    AudioChannel* channel;
} _Channel_obj_t;


/***** Print *****/
void Channel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    //_Channel_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Channel_obj_t);
    //AudioChannel* channel = self->channel;
    mp_print_str(print, "Channel(");
    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Channel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_raise_msg(&mp_type_RuntimeError, "Cannot create Channel objects. They can only be accessed from GalacticUnicorn.synth_channel()");
    return mp_const_none;
}


/***** Destructor ******/
mp_obj_t Channel___del__(mp_obj_t self_in) {
    return mp_const_none;
}


/***** Methods *****/
mp_obj_t Channel_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_waveforms, ARG_attack_ms, ARG_decay_ms, ARG_sustain, ARG_release_ms, ARG_volume };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_waveforms, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_attack_ms, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_decay_ms, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sustain, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_release_ms, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_volumes, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Channel_obj_t);

    self->channel->waveforms = args[ARG_waveforms].u_int;
    self->channel->attack_ms = args[ARG_attack_ms].u_int;
    self->channel->decay_ms = args[ARG_decay_ms].u_int;
    self->channel->sustain = args[ARG_sustain].u_int;
    self->channel->release_ms = args[ARG_release_ms].u_int;
    self->channel->volume = args[ARG_volume].u_int;

    return mp_const_none;
}

mp_obj_t Channel_freq(mp_obj_t self_in, mp_obj_t freq_in) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Channel_obj_t);

    float freq = mp_obj_get_float(freq_in);
    if(freq <= 0.0f) {
        mp_raise_ValueError("freq out of range. Expected greater than 0.0");
    }
    self->channel->frequency = freq;

    return mp_const_none;
}

mp_obj_t Channel_trigger_attack(mp_obj_t self_in) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Channel_obj_t);
    self->channel->trigger_attack();

    return mp_const_none;
}

mp_obj_t Channel_trigger_release(mp_obj_t self_in) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Channel_obj_t);
    self->channel->trigger_release();

    return mp_const_none;
}

mp_obj_t Channel_play_tone(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_freq, ARG_volume, ARG_fade_in, ARG_fade_out };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_volume, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_fade_in, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_fade_out, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Channel_obj_t);

    float freq = mp_obj_get_float(args[ARG_freq].u_obj);
    if(freq <= 0.0f) {
        mp_raise_ValueError("freq out of range. Expected greater than 0.0");
    }

    float volume = 1.0f;
    if(args[ARG_volume].u_obj != mp_const_none) {
        volume = mp_obj_get_float(args[ARG_volume].u_obj);
        if(volume < 0.0f || volume > 1.0f) {
            mp_raise_ValueError("volume out of range. Expected 0.0 to 1.0");
        }
    }

    int fade_in_ms = 1;
    if(args[ARG_fade_in].u_obj != mp_const_none) {
        float fade_in = mp_obj_get_float(args[ARG_fade_in].u_obj);
        if(fade_in <= 0.0f) {
            mp_raise_ValueError("fade_in out of range. Expected greater than 0.0");
        }
        fade_in_ms = (uint16_t)(fade_in * 1000.0f);
    }

    int fade_out_ms = 1;
    if(args[ARG_fade_out].u_obj != mp_const_none) {
        float fade_out = mp_obj_get_float(args[ARG_fade_out].u_obj);
        if(fade_out <= 0.0f) {
            mp_raise_ValueError("fade_out out of range. Expected greater than 0.0");
        }
        fade_out_ms = (uint16_t)(fade_out * 1000.0f);
    }


    self->channel->frequency = freq;
    self->channel->waveforms = Waveform::SINE;
    self->channel->attack_ms = MAX(fade_in_ms, 1);
    self->channel->decay_ms = 1;
    self->channel->sustain = 0xffff;
    self->channel->release_ms = MAX(fade_out_ms, 1);
    self->channel->volume = (uint16_t)(volume * 0xffff);

    self->channel->trigger_attack();

    return mp_const_none;
}


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

extern mp_obj_t GalacticUnicorn_play_synth(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->play_synth();

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_stop_playing(mp_obj_t self_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);
    self->galactic->stop_playing();

    return mp_const_none;
}

extern mp_obj_t GalacticUnicorn_synth_channel(mp_obj_t self_in, mp_obj_t channel_in) {
    _GalacticUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _GalacticUnicorn_obj_t);

    // Check that the channel is valid
    int channel = mp_obj_get_int(channel_in);
    if(channel < 0 || channel >= (int)PicoSynth::CHANNEL_COUNT) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("channel out of range. Expected 0 to %d"), PicoSynth::CHANNEL_COUNT - 1);
    }

    // NOTE This seems to work, in that it give MP access to the calibration object
    // Could very easily mess up in weird ways once object deletion is considered?
    _Channel_obj_t *channel_obj = m_new_obj_with_finaliser(_Channel_obj_t);
    channel_obj->base.type = &Channel_type;
    channel_obj->channel = &self->galactic->synth_channel(channel);

    return MP_OBJ_FROM_PTR(channel_obj);
}
}
