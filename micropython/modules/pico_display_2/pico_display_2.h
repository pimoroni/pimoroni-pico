// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picodisplay2_init(mp_obj_t buf_obj);
extern mp_obj_t picodisplay2_get_width();
extern mp_obj_t picodisplay2_get_height();
extern mp_obj_t picodisplay2_set_backlight(mp_obj_t brightness_obj);
extern mp_obj_t picodisplay2_update();
extern mp_obj_t picodisplay2_set_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picodisplay2_is_pressed(mp_obj_t button_obj);
extern mp_obj_t picodisplay2_flip();

// From PicoGraphics parent class
extern mp_obj_t picodisplay2_set_pen(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picodisplay2_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picodisplay2_set_clip(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picodisplay2_remove_clip();
extern mp_obj_t picodisplay2_clear();
extern mp_obj_t picodisplay2_pixel(mp_obj_t x_obj, mp_obj_t y_obj);
extern mp_obj_t picodisplay2_pixel_span(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t l_obj);
extern mp_obj_t picodisplay2_rectangle(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picodisplay2_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picodisplay2_character(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picodisplay2_text(mp_uint_t n_args, const mp_obj_t *args);
