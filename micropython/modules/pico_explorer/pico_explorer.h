// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picoexplorer_init(mp_obj_t buf_obj);
extern mp_obj_t picoexplorer_get_width();
extern mp_obj_t picoexplorer_get_height();
extern mp_obj_t picoexplorer_set_backlight(mp_obj_t brightness_obj);
extern mp_obj_t picoexplorer_update();
extern mp_obj_t picoexplorer_is_pressed(mp_obj_t button_obj);
extern mp_obj_t picoexplorer_get_adc(mp_obj_t channel_obj);
extern mp_obj_t picoexplorer_set_motor(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoexplorer_set_audio_pin(mp_obj_t pin_obj);
extern mp_obj_t picoexplorer_set_tone(mp_uint_t n_args, const mp_obj_t *args);

// From PicoGraphics parent class
extern mp_obj_t picoexplorer_set_pen(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoexplorer_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picoexplorer_set_clip(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoexplorer_remove_clip();
extern mp_obj_t picoexplorer_clear();
extern mp_obj_t picoexplorer_pixel(mp_obj_t x_obj, mp_obj_t y_obj);
extern mp_obj_t picoexplorer_pixel_span(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t l_obj);
extern mp_obj_t picoexplorer_rectangle(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoexplorer_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picoexplorer_character(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picoexplorer_text(mp_uint_t n_args, const mp_obj_t *args);