// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
static const int WIDTH = 240;
static const int HEIGHT = 240;


/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_roundlcd_BreakoutRoundLCD_type;

/***** Extern of Class Methods *****/
extern void BreakoutRoundLCD_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutRoundLCD_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t  BreakoutRoundLCD_update(mp_obj_t self_in);
extern mp_obj_t  BreakoutRoundLCD_set_backlight(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t  BreakoutRoundLCD_set_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_create_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_set_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_remove_clip(mp_obj_t self_in);
extern mp_obj_t  BreakoutRoundLCD_clear(mp_obj_t self_in);
extern mp_obj_t  BreakoutRoundLCD_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_pixel_span(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_triangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutRoundLCD_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);