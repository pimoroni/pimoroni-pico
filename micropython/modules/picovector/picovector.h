#include "py/runtime.h"
#include "py/objstr.h"

extern const mp_obj_type_t VECTOR_type;
extern const mp_obj_type_t POLYGON_type;
extern const mp_obj_type_t REGULAR_POLYGON_type;
extern const mp_obj_type_t RECTANGLE_type;

extern mp_obj_t POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t REGULAR_POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t RECTANGLE_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern void POLYGON_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t POLYGON_centroid(mp_obj_t self_in);
extern mp_obj_t POLYGON_bounds(mp_obj_t self_in);
extern mp_obj_t POLYGON_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf);

extern mp_obj_t POLYGON__del__(mp_obj_t self_in);

extern mp_obj_t VECTOR_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t VECTOR_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t VECTOR_set_font(mp_obj_t self_in, mp_obj_t font, mp_obj_t size);
extern mp_obj_t VECTOR_set_font_size(mp_obj_t self_in, mp_obj_t size);
extern mp_obj_t VECTOR_set_antialiasing(mp_obj_t self_in, mp_obj_t aa);

extern mp_obj_t VECTOR_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t VECTOR_rotate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t VECTOR_translate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);