// Include MicroPython API.
#include "py/runtime.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picokeypad_init();
extern mp_obj_t picokeypad_get_width();
extern mp_obj_t picokeypad_get_height();
extern mp_obj_t picokeypad_get_num_pads();
extern mp_obj_t picokeypad_update();
extern mp_obj_t picokeypad_set_brightness(mp_obj_t brightness_obj);
extern mp_obj_t picokeypad_illuminate_xy(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picokeypad_illuminate(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picokeypad_clear();
extern mp_obj_t picokeypad_get_button_states();