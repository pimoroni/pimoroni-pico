#include "py/runtime.h"

extern const mp_obj_type_t MLX90640_type;

extern mp_obj_t MLX90640_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t MLX90640_get_frame(mp_obj_t self_in);
extern mp_obj_t MLX90640_setup(mp_obj_t self_in, mp_obj_t fps_in);