#include "libraries/stellar_unicorn/stellar_unicorn.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>
#include <cfloat>


using namespace pimoroni;

extern "C" {
#include "stellar_unicorn.h"
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
    mp_raise_msg(&mp_type_RuntimeError, "Cannot create Channel objects. They can only be accessed from StellarUnicorn.synth_channel()");
    return mp_const_none;
}


/***** Destructor ******/
mp_obj_t Channel___del__(mp_obj_t self_in) {
    return mp_const_none;
}


/***** Helper Functions *****/
void set_channel_waveforms(AudioChannel& channel, mp_obj_t in) {
    int waveforms = mp_obj_get_int(in);
    const int mask = (NOISE | SQUARE | SAW | TRIANGLE | SINE | WAVE);
    if(waveforms < 0 || (waveforms & mask) == 0) {
        mp_raise_ValueError("waveforms invalid. Expected a combination of NOISE, SQUARE, SAW, TRIANGLE, SINE, or WAVE");
    }
    channel.waveforms = (uint8_t)waveforms;
}

void set_channel_frequency(AudioChannel& channel, mp_obj_t in) {
    int freq = mp_obj_get_int(in);
    if(freq <= 0 || freq > UINT16_MAX) {
        mp_raise_ValueError("frequency out of range. Expected greater than 0Hz to 65535Hz");
    }
    channel.frequency = (uint16_t)freq;
}

void set_channel_volume(AudioChannel& channel, mp_obj_t in) {
    float volume = mp_obj_get_float(in);
    if(volume < 0.0f || volume > 1.0f) {
        mp_raise_ValueError("volume out of range. Expected 0.0 to 1.0");
    }
    channel.volume = (uint16_t)(volume * UINT16_MAX);
}

void set_channel_attack(AudioChannel& channel, mp_obj_t in) {
    int attack_ms = (int)(mp_obj_get_float(in) * 1000.0f);
    if(attack_ms < 0 || attack_ms > UINT16_MAX) {
        mp_raise_ValueError("attack out of range. Expected 0.0s to 65.5s");
    }
    channel.attack_ms = MAX(attack_ms, 1);
}

void set_channel_decay(AudioChannel& channel, mp_obj_t in) {
    int decay_ms = (int)(mp_obj_get_float(in) * 1000.0f);
    if(decay_ms < 0 || decay_ms > UINT16_MAX) {
        mp_raise_ValueError("decay out of range. Expected 0.0s to 65.5s");
    }
    channel.decay_ms = MAX(decay_ms, 1);
}

void set_channel_sustain(AudioChannel& channel, mp_obj_t in) {
    float sustain = mp_obj_get_float(in);
    if(sustain < 0.0f || sustain > 1.0f) {
        mp_raise_ValueError("sustain out of range. Expected 0.0 to 1.0");
    }
    channel.sustain = (uint16_t)(sustain * UINT16_MAX);
}

void set_channel_release(AudioChannel& channel, mp_obj_t in) {
    int release_ms = (int)(mp_obj_get_float(in) * 1000.0f);
    if(release_ms < 0 || release_ms > UINT16_MAX) {
        mp_raise_ValueError("release out of range. Expected 0.0s to 65.5s");
    }
    channel.release_ms = MAX(release_ms, 1);
}

void set_channel_pulse_width(AudioChannel& channel, mp_obj_t in) {
    float pulse_width = mp_obj_get_float(in);
    if(pulse_width < 0.0f || pulse_width > 1.0f) {
        mp_raise_ValueError("pulse_width out of range. Expected 0.0 to 1.0");
    }
    channel.pulse_width = (uint16_t)(pulse_width * UINT16_MAX);
}


/***** Methods *****/
mp_obj_t Channel_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_waveforms, ARG_frequency, ARG_volume, ARG_attack, ARG_decay, ARG_sustain, ARG_release, ARG_pulse_width };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_waveforms, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_frequency, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_volume, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_attack, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_decay, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_sustain, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_release, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_pulse_width, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Channel_obj_t);

    mp_obj_t waveforms = args[ARG_waveforms].u_obj;
    if(waveforms != mp_const_none) {
        set_channel_waveforms(*self->channel, waveforms);
    }

    mp_obj_t frequency = args[ARG_frequency].u_obj;
    if(frequency != mp_const_none) {
        set_channel_frequency(*self->channel, frequency);
    }

    mp_obj_t volume = args[ARG_volume].u_obj;
    if(volume != mp_const_none) {
        set_channel_volume(*self->channel, volume);
    }

    mp_obj_t attack = args[ARG_attack].u_obj;
    if(attack != mp_const_none) {
        set_channel_attack(*self->channel, attack);
    }

    mp_obj_t decay = args[ARG_decay].u_obj;
    if(decay != mp_const_none) {
        set_channel_decay(*self->channel, decay);
    }

    mp_obj_t sustain = args[ARG_sustain].u_obj;
    if(sustain != mp_const_none) {
        set_channel_sustain(*self->channel, sustain);
    }

    mp_obj_t release = args[ARG_release].u_obj;
    if(release != mp_const_none) {
        set_channel_release(*self->channel, release);
    }

    mp_obj_t pulse_width = args[ARG_pulse_width].u_obj;
    if(pulse_width != mp_const_none) {
        set_channel_pulse_width(*self->channel, pulse_width);
    }

    return mp_const_none;
}

mp_obj_t Channel_restore(mp_obj_t self_in) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Channel_obj_t);
    self->channel->restore();
    return mp_const_none;
}

mp_obj_t Channel_waveforms(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_int(self->channel->waveforms);
    }

    set_channel_waveforms(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_frequency(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_int(self->channel->frequency);
    }

    set_channel_frequency(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_volume(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->volume / UINT16_MAX);
    }

    set_channel_volume(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_attack_duration(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->attack_ms / 1000.0f);
    }

    set_channel_attack(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_decay_duration(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->decay_ms / 1000.0f);
    }

    set_channel_decay(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_sustain_level(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->sustain / UINT16_MAX);
    }

    set_channel_sustain(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_release_duration(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->release_ms / 1000.0f);
    }

    set_channel_release(*self->channel, args[1]);
    return mp_const_none;
}

mp_obj_t Channel_pulse_width(size_t n_args, const mp_obj_t *args) {
    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[0], _Channel_obj_t);

    if(n_args == 1) {
        return mp_obj_new_float((float)self->channel->pulse_width / 0xffff);
    }

    set_channel_pulse_width(*self->channel, args[1]);
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
        { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_volume, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_attack, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_release, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Channel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Channel_obj_t);

    set_channel_frequency(*self->channel, args[ARG_freq].u_obj);

    mp_obj_t volume = args[ARG_volume].u_obj;
    if(volume != mp_const_none) {
        set_channel_volume(*self->channel, volume);
    }
    else {
        self->channel->volume = UINT16_MAX;
    }

    mp_obj_t attack_ms = args[ARG_fade_in].u_obj;
    if(attack_ms != mp_const_none) {
        set_channel_attack(*self->channel, attack_ms);
    }
    else {
        self->channel->attack_ms = 1;
    }

    mp_obj_t release_ms = args[ARG_fade_out].u_obj;
    if(release_ms != mp_const_none) {
        set_channel_release(*self->channel, release_ms);
    }
    else {
        self->channel->release_ms = 1;
    }

    self->channel->waveforms = Waveform::SINE;
    self->channel->decay_ms = 1;
    self->channel->sustain = UINT16_MAX;

    self->channel->trigger_attack();

    return mp_const_none;
}


/********** StellarUnicorn **********/

/***** Variables Struct *****/
typedef struct _StellarUnicorn_obj_t {
    mp_obj_base_t base;
    StellarUnicorn* stellar;
} _StellarUnicorn_obj_t;

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
void StellarUnicorn_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    //_StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    mp_print_str(print, "StellarUnicorn()");
}


/***** Constructor *****/
mp_obj_t StellarUnicorn_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _StellarUnicorn_obj_t *self = nullptr;

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


    StellarUnicorn *stellar = m_new_class(StellarUnicorn);
    stellar->init();

    self = m_new_obj_with_finaliser(_StellarUnicorn_obj_t);
    self->base.type = &StellarUnicorn_type;
    self->stellar = stellar;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t StellarUnicorn___del__(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    m_del_class(StellarUnicorn, self->stellar);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t StellarUnicorn_clear(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->clear();
    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_update(mp_obj_t self_in, mp_obj_t graphics_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    ModPicoGraphics_obj_t *picographics = MP_OBJ_TO_PTR2(graphics_in, ModPicoGraphics_obj_t);

    self->stellar->update(picographics->graphics);

    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_set_brightness(mp_obj_t self_in, mp_obj_t value) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->set_brightness(mp_obj_get_float(value));
    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_get_brightness(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    return mp_obj_new_float(self->stellar->get_brightness());
}

extern mp_obj_t StellarUnicorn_adjust_brightness(mp_obj_t self_in, mp_obj_t delta) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->adjust_brightness(mp_obj_get_float(delta));
    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_set_volume(mp_obj_t self_in, mp_obj_t value) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->set_volume(mp_obj_get_float(value));
    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_get_volume(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    return mp_obj_new_float(self->stellar->get_volume());
}

extern mp_obj_t StellarUnicorn_adjust_volume(mp_obj_t self_in, mp_obj_t delta) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->adjust_volume(mp_obj_get_float(delta));
    return mp_const_none;
}


extern mp_obj_t StellarUnicorn_light(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    return mp_obj_new_float(self->stellar->light());
}

extern mp_obj_t StellarUnicorn_is_pressed(mp_obj_t self_in, mp_obj_t button) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    return mp_obj_new_bool(self->stellar->is_pressed((uint8_t)mp_obj_get_int(button)));
}

extern mp_obj_t StellarUnicorn_play_sample(mp_obj_t self_in, mp_obj_t data) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_RW);
    if(bufinfo.len < 1) {
        mp_raise_ValueError("Supplied buffer is too small!");
    }

    self->stellar->play_sample((uint8_t *)bufinfo.buf, bufinfo.len);

    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_play_synth(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->play_synth();

    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_stop_playing(mp_obj_t self_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);
    self->stellar->stop_playing();

    return mp_const_none;
}

extern mp_obj_t StellarUnicorn_synth_channel(mp_obj_t self_in, mp_obj_t channel_in) {
    _StellarUnicorn_obj_t *self = MP_OBJ_TO_PTR2(self_in, _StellarUnicorn_obj_t);

    // Check that the channel is valid
    int channel = mp_obj_get_int(channel_in);
    if(channel < 0 || channel >= (int)PicoSynth::CHANNEL_COUNT) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("channel out of range. Expected 0 to %d"), PicoSynth::CHANNEL_COUNT - 1);
    }

    // NOTE This seems to work, in that it give MP access to the calibration object
    // Could very easily mess up in weird ways once object deletion is considered?
    _Channel_obj_t *channel_obj = m_new_obj_with_finaliser(_Channel_obj_t);
    channel_obj->base.type = &Channel_type;
    channel_obj->channel = &self->stellar->synth_channel(channel);

    return MP_OBJ_FROM_PTR(channel_obj);
}
}
