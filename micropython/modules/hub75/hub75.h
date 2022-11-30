// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Hub75_type;

/***** Extern of Class Methods *****/
extern void Hub75_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Hub75_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Hub75___del__(mp_obj_t self_in);
extern mp_obj_t Hub75_start(mp_obj_t self_in);
extern mp_obj_t Hub75_stop(mp_obj_t self_in);
extern mp_obj_t Hub75_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Hub75_clear(mp_obj_t self_in);
extern mp_obj_t Hub75_update(mp_obj_t self_in, mp_obj_t graphics_in);