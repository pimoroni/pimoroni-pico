#include "py/runtime.h"
#include "py/objstr.h"

enum ST7789Display {
    DISPLAY_LCD_240X240=0,
    DISPLAY_ROUND_LCD_240X240,
    DISPLAY_PICO_DISPLAY,
    DISPLAY_PICO_DISPLAY_2,
    DISPLAY_PICO_EXPLORER,
    DISPLAY_TUFTY_2040,
    DISPLAY_ENVIRO_PLUS
};

// Type
extern const mp_obj_type_t GenericST7789_type;

// Module functions
extern mp_obj_t GenericST7789_module_RGB332(mp_obj_t r, mp_obj_t g, mp_obj_t b);
extern mp_obj_t GenericST7789_module_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b);

// Class methods
extern mp_obj_t GenericST7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t GenericST7789_update(mp_obj_t self_in);
extern mp_obj_t GenericST7789_set_backlight(mp_obj_t self_in, mp_obj_t brightness);

// Palette management
extern mp_obj_t GenericST7789_update_pen(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_reset_pen(mp_obj_t self_in, mp_obj_t pen);

// Pen
extern mp_obj_t GenericST7789_set_pen(mp_obj_t self_in, mp_obj_t pen);
extern mp_obj_t GenericST7789_create_pen(size_t n_args, const mp_obj_t *args);

// Primitives
extern mp_obj_t GenericST7789_set_clip(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_remove_clip(mp_obj_t self_in);
extern mp_obj_t GenericST7789_clear(mp_obj_t self_in);
extern mp_obj_t GenericST7789_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t GenericST7789_pixel_span(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_rectangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_circle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t GenericST7789_triangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t GenericST7789_line(size_t n_args, const mp_obj_t *args);

// Utility
extern mp_obj_t GenericST7789_get_bounds(mp_obj_t self_in);
extern mp_obj_t GenericST7789_set_framebuffer(mp_obj_t self_in, mp_obj_t framebuffer);