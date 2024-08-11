// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

extern const mp_obj_type_t PicoScroll_type;
extern const mp_obj_type_t ModPicoGraphics_type;

// Declare the functions we'll make available in Python
extern mp_obj_t picoscroll_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t picoscroll___del__(mp_obj_t self_in);
extern mp_obj_t picoscroll_get_width(mp_obj_t self_in);
extern mp_obj_t picoscroll_get_height(mp_obj_t self_in);
extern mp_obj_t picoscroll_show(mp_obj_t self_in);
extern mp_obj_t picoscroll_set_pixel(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoscroll_set_pixels(mp_obj_t self_in, mp_obj_t image_obj);
extern mp_obj_t picoscroll_show_text(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoscroll_scroll_text(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoscroll_show_bitmap_1d(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoscroll_clear(mp_obj_t self_in);
extern mp_obj_t picoscroll_is_pressed(mp_obj_t self_in, mp_obj_t button_obj);

extern mp_obj_t picoscroll_update(mp_obj_t self_in, mp_obj_t graphics_in);