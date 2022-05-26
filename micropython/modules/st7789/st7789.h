// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t GenericST7789_type;

/***** Extern of Class Methods *****/
extern void GenericST7789_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t GenericST7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t GenericST7789Parallel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t GenericST7789_update(mp_obj_t self_in);
extern mp_obj_t GenericST7789_flush_palette(mp_obj_t self_in);
extern mp_obj_t GenericST7789_default_palette(mp_obj_t self_in);
extern mp_obj_t GenericST7789_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t GenericST7789_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_remove_clip(mp_obj_t self_in);
extern mp_obj_t GenericST7789_clear(mp_obj_t self_in);
extern mp_obj_t GenericST7789_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);