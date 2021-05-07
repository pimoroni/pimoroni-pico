// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
static const int WIDTH = 160;
static const int HEIGHT = 80;


/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_colourlcd160x80_BreakoutColourLCD160x80_type;

/***** Extern of Class Methods *****/
extern void BreakoutColourLCD160x80_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutColourLCD160x80_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t  BreakoutColourLCD160x80_update(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD160x80_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t  BreakoutColourLCD160x80_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_remove_clip(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD160x80_clear(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD160x80_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD160x80_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);