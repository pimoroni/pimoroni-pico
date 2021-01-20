// Include MicroPython API.
#include "py/runtime.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picodisplay_init();
extern mp_obj_t picodisplay_get_width();
extern mp_obj_t picodisplay_get_height();
extern mp_obj_t picodisplay_set_backlight(mp_obj_t brightness_obj);
extern mp_obj_t picodisplay_update();
extern mp_obj_t picodisplay_set_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picodisplay_is_pressed(mp_obj_t button_obj);

// From PicoGraphics parent class
extern mp_obj_t picodisplay_set_pen_rgb(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picodisplay_set_pen(mp_obj_t p_obj);
extern mp_obj_t picodisplay_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);

extern mp_obj_t picodisplay_set_clip(mp_obj_t r_obj);
extern mp_obj_t picodisplay_remove_clip();

//extern mp_obj_t picodisplay_ptr(mp_obj_t p_obj);
//extern mp_obj_t picodisplay_ptr(mp_obj_t r_obj);
//extern mp_obj_t picodisplay_ptr(mp_obj_t x_obj, mp_obj_t y_obj);

extern mp_obj_t picodisplay_clear();
extern mp_obj_t picodisplay_pixel(mp_obj_t p_obj);
extern mp_obj_t picodisplay_pixel_span(mp_obj_t p_obj, mp_obj_t l_obj);
extern mp_obj_t picodisplay_rectangle(mp_obj_t r_obj);
extern mp_obj_t picodisplay_circle(mp_obj_t p_obj, mp_obj_t r_obj);
extern mp_obj_t picodisplay_text(mp_obj_t t_obj, mp_obj_t p_obj, mp_obj_t wrap_obj);
extern mp_obj_t picodisplay_character(mp_obj_t c_obj, mp_obj_t p_obj, mp_obj_t scale_obj);