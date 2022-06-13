// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/

/***** Extern of Class Definition *****/
extern const mp_obj_type_t adcfft_type;

/***** Extern of Class Methods *****/
extern mp_obj_t adcfft_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t adcfft__del__(mp_obj_t self_in);

extern mp_obj_t adcfft_update(mp_obj_t self_in);
extern mp_obj_t adcfft_get_scaled(mp_obj_t self_in, mp_obj_t index, mp_obj_t scale);