// Include MicroPython API.
#include "py/runtime.h"

extern const mp_obj_type_t PicoKeypad_type;

// Declare the functions we'll make available in Python
extern mp_obj_t picokeypad_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t picokeypad___del__(mp_obj_t self_in);
extern mp_obj_t picokeypad_get_width(mp_obj_t self_in);
extern mp_obj_t picokeypad_get_height(mp_obj_t self_in);
extern mp_obj_t picokeypad_get_num_pads(mp_obj_t self_in);
extern mp_obj_t picokeypad_update(mp_obj_t self_in);
extern mp_obj_t picokeypad_set_brightness(mp_obj_t self_in, mp_obj_t brightness_obj);
extern mp_obj_t picokeypad_illuminate_xy(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picokeypad_illuminate(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picokeypad_clear(mp_obj_t self_in);
extern mp_obj_t picokeypad_get_button_states(mp_obj_t self_in);