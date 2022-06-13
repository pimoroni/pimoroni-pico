#include "libraries/adcfft/adcfft.hpp"
#include "micropython/modules/util.hpp"

extern "C" {
#include "adcfft.h"

typedef struct _adcfft_obj_t {
    mp_obj_base_t base;
    ADCFFT *adcfft;
} adcfft_obj_t;

mp_obj_t adcfft_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_adc_channel, ARG_adc_gpio, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_adc_channel, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_adc_gpio, MP_ARG_INT, {.u_int = 26} },
        { MP_QSTR_sample_rate, MP_ARG_INT, {.u_int = 10000u} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    adcfft_obj_t *self = m_new_obj_with_finaliser(adcfft_obj_t);
    self->base.type = &adcfft_type;

    unsigned int adc_channel = args[ARG_adc_channel].u_int;
    unsigned int adc_gpio = args[ARG_adc_gpio].u_int;
    float sample_rate = (float)args[ARG_sample_rate].u_int;

    self->adcfft = new(m_new(ADCFFT, 1)) ADCFFT(adc_channel, adc_gpio, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t adcfft__del__(mp_obj_t self_in) {
    adcfft_obj_t *self = MP_OBJ_TO_PTR2(self_in, adcfft_obj_t);
    self->adcfft->~ADCFFT();
    m_del(ADCFFT, self->adcfft, 1);
    return mp_const_none;
}

mp_obj_t adcfft_update(mp_obj_t self_in) {
    adcfft_obj_t *self = MP_OBJ_TO_PTR2(self_in, adcfft_obj_t);
    self->adcfft->update();
    return mp_const_none;
}

mp_obj_t adcfft_get_scaled(mp_obj_t self_in, mp_obj_t index, mp_obj_t scale) {
    adcfft_obj_t *self = MP_OBJ_TO_PTR2(self_in, adcfft_obj_t);
    return mp_obj_new_int(self->adcfft->get_scaled(mp_obj_get_int(index), mp_obj_get_int(scale)));
}
}