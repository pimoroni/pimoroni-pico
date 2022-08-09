// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t GalacticUnicorn_type;

/***** Extern of Class Methods *****/
extern void GalacticUnicorn_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t GalacticUnicorn_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t GalacticUnicorn___del__(mp_obj_t self_in);
extern mp_obj_t GalacticUnicorn_clear(mp_obj_t self_in);

extern mp_obj_t GalacticUnicorn_update(mp_obj_t self_in, mp_obj_t graphics_in);

extern mp_obj_t GalacticUnicorn_set_brightness(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t GalacticUnicorn_get_brightness(mp_obj_t self_in);
extern mp_obj_t GalacticUnicorn_adjust_brightness(mp_obj_t self_in, mp_obj_t delta);

extern mp_obj_t GalacticUnicorn_set_volume(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t GalacticUnicorn_get_volume(mp_obj_t self_in);
extern mp_obj_t GalacticUnicorn_adjust_volume(mp_obj_t self_in, mp_obj_t delta);

extern mp_obj_t GalacticUnicorn_light(mp_obj_t self_in);

extern mp_obj_t GalacticUnicorn_is_pressed(mp_obj_t self_in, mp_obj_t button);

extern mp_obj_t GalacticUnicorn_play_sample(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);