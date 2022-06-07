// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

static const int WIDTH = 160;
static const int HEIGHT = 80;

// Type
extern const mp_obj_type_t BreakoutColourLCD160x80_type;

// Module functions
extern mp_obj_t BreakoutColourLCD160x80_module_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b);

// Class methods
extern mp_obj_t BreakoutColourLCD160x80_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t BreakoutColourLCD160x80_update(mp_obj_t self_in);
extern mp_obj_t BreakoutColourLCD160x80_set_backlight(mp_obj_t self_in, mp_obj_t brightness);

// Pen
extern mp_obj_t BreakoutColourLCD160x80_set_pen(mp_obj_t self_in, mp_obj_t pen);
extern mp_obj_t BreakoutColourLCD160x80_create_pen(size_t n_args, const mp_obj_t *args);

// Primitives
extern mp_obj_t BreakoutColourLCD160x80_set_clip(size_t n_args, const mp_obj_t *args);
extern mp_obj_t BreakoutColourLCD160x80_remove_clip(mp_obj_t self_in);
extern mp_obj_t BreakoutColourLCD160x80_clear(mp_obj_t self_in);
extern mp_obj_t BreakoutColourLCD160x80_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t BreakoutColourLCD160x80_pixel_span(size_t n_args, const mp_obj_t *args);
extern mp_obj_t BreakoutColourLCD160x80_rectangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t BreakoutColourLCD160x80_circle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t BreakoutColourLCD160x80_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutColourLCD160x80_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutColourLCD160x80_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutColourLCD160x80_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutColourLCD160x80_triangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t BreakoutColourLCD160x80_line(size_t n_args, const mp_obj_t *args);

// Utility
extern mp_obj_t BreakoutColourLCD160x80_get_bounds(mp_obj_t self_in);