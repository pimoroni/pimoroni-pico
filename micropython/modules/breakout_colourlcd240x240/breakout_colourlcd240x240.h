// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
static const int WIDTH = 240;
static const int HEIGHT = 240;


/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_colourlcd240x240_BreakoutColourLCD240x240_type;

/***** Extern of Class Methods *****/
extern void BreakoutColourLCD240x240_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutColourLCD240x240_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t  BreakoutColourLCD240x240_update(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD240x240_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t  BreakoutColourLCD240x240_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_remove_clip(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD240x240_clear(mp_obj_t self_in);
extern mp_obj_t  BreakoutColourLCD240x240_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutColourLCD240x240_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);