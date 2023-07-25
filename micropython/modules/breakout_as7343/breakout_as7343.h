// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/


/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_as7343_BreakoutAS7343_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutAS7343_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutAS7343_reset(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7343_version(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7343_read(mp_obj_t self_in);

extern mp_obj_t BreakoutAS7343_set_gain(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7343_set_measurement_time(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7343_set_integration_time(mp_obj_t self_in, mp_obj_t value_in);

extern mp_obj_t BreakoutAS7343_set_illumination_current(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7343_set_illumination_led(mp_obj_t self_in, mp_obj_t value_in);

extern mp_obj_t BreakoutAS7343_set_channels(mp_obj_t self_in, mp_obj_t value_in);