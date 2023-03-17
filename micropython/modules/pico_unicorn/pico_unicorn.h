#include "py/runtime.h"

extern const mp_obj_type_t picounicorn_type;
extern const mp_obj_type_t ModPicoGraphics_type;

extern mp_obj_t picounicorn_get_width(mp_obj_t self_in);
extern mp_obj_t picounicorn_get_height(mp_obj_t self_in);

extern mp_obj_t picounicorn__del__(mp_obj_t self_in);
extern mp_obj_t picounicorn_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t picounicorn_set_pixel(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picounicorn_set_pixel_value(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picounicorn_clear(mp_obj_t self_in);
extern mp_obj_t picounicorn_is_pressed(mp_obj_t self_in, mp_obj_t button_obj);

extern mp_obj_t picounicorn_update(mp_obj_t self_in, mp_obj_t graphics_in);