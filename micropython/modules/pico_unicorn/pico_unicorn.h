// Include MicroPython API.
//#include "py/obj.h"
#include "py/runtime.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picounicorn_init();
extern mp_obj_t picounicorn_get_width();
extern mp_obj_t picounicorn_get_height();
//extern mp_obj_t picounicorn_update();
extern mp_obj_t picounicorn_set_pixel(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picounicorn_set_pixel_value(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj);
extern mp_obj_t picounicorn_clear();
extern mp_obj_t picounicorn_is_pressed(mp_obj_t button_obj);