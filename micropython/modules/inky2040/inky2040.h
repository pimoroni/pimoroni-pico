// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Inky2040_type;

/***** Extern of Class Methods *****/
extern void Inky2040_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Inky2040___del__(mp_obj_t self_in);
extern mp_obj_t Inky2040_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t Inky2040_is_busy(mp_obj_t self_in);
extern mp_obj_t Inky2040_update(mp_obj_t self_in);

extern mp_obj_t Inky2040_font(mp_obj_t self_in, mp_obj_t font);
extern mp_obj_t Inky2040_pen(mp_obj_t self_in, mp_obj_t color);
extern mp_obj_t Inky2040_thickness(mp_obj_t self_in, mp_obj_t thickness);

extern mp_obj_t Inky2040_clear(mp_obj_t self_in);
extern mp_obj_t Inky2040_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t Inky2040_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Inky2040_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Inky2040_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Inky2040_icon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Inky2040_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Inky2040_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Inky2040_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Inky2040_measure_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
