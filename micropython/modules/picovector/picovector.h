#include "py/runtime.h"
#include "py/objstr.h"

extern const mp_obj_type_t VECTOR_type;

extern mp_obj_t VECTOR_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t VECTOR_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t VECTOR_regular_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);