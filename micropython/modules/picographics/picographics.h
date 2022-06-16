#include "py/runtime.h"
#include "py/objstr.h"

enum PicoGraphicsDisplay {
    DISPLAY_LCD_240X240=0,
    DISPLAY_ROUND_LCD_240X240,
    DISPLAY_PICO_DISPLAY,
    DISPLAY_PICO_DISPLAY_2,
    DISPLAY_PICO_EXPLORER,
    DISPLAY_TUFTY_2040,
    DISPLAY_ENVIRO_PLUS,
    DISPLAY_LCD_160X80,
    DISPLAY_I2C_OLED_128X128
};

enum PicoGraphicsPenType {
    PEN_1BIT = 0,
    PEN_P2,
    PEN_P4,
    PEN_P8,
    PEN_RGB332,
    PEN_RGB565
};

// Type
extern const mp_obj_type_t ModPicoGraphics_type;

// Module functions
extern mp_obj_t ModPicoGraphics_module_RGB_to_RGB332(mp_obj_t r, mp_obj_t g, mp_obj_t b);
extern mp_obj_t ModPicoGraphics_module_RGB_to_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b);
extern mp_obj_t ModPicoGraphics_module_RGB332_to_RGB(mp_obj_t rgb332);
extern mp_obj_t ModPicoGraphics_module_RGB565_to_RGB(mp_obj_t rgb565);
extern mp_obj_t ModPicoGraphics_get_required_buffer_size(mp_obj_t display_in, mp_obj_t pen_type_in);

// Class methods
extern mp_obj_t ModPicoGraphics_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t ModPicoGraphics_update(mp_obj_t self_in);
extern mp_obj_t ModPicoGraphics_set_backlight(mp_obj_t self_in, mp_obj_t brightness);

// Palette management
extern mp_obj_t ModPicoGraphics_update_pen(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_reset_pen(mp_obj_t self_in, mp_obj_t pen);
extern mp_obj_t ModPicoGraphics_set_palette(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

// Pen
extern mp_obj_t ModPicoGraphics_set_pen(mp_obj_t self_in, mp_obj_t pen);
extern mp_obj_t ModPicoGraphics_create_pen(size_t n_args, const mp_obj_t *args);

// Primitives
extern mp_obj_t ModPicoGraphics_set_clip(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_remove_clip(mp_obj_t self_in);
extern mp_obj_t ModPicoGraphics_clear(mp_obj_t self_in);
extern mp_obj_t ModPicoGraphics_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t ModPicoGraphics_pixel_span(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_rectangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_circle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ModPicoGraphics_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ModPicoGraphics_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ModPicoGraphics_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ModPicoGraphics_triangle(size_t n_args, const mp_obj_t *args);
extern mp_obj_t ModPicoGraphics_line(size_t n_args, const mp_obj_t *args);

// Sprites
extern mp_obj_t ModPicoGraphics_set_spritesheet(mp_obj_t self_in, mp_obj_t spritedata);
extern mp_obj_t ModPicoGraphics_load_spritesheet(mp_obj_t self_in, mp_obj_t filename);
extern mp_obj_t ModPicoGraphics_sprite(size_t n_args, const mp_obj_t *args);

// Utility
//extern mp_obj_t ModPicoGraphics_set_scanline_callback(mp_obj_t self_in, mp_obj_t cb_in);
extern mp_obj_t ModPicoGraphics_set_font(mp_obj_t self_in, mp_obj_t font);
extern mp_obj_t ModPicoGraphics_get_bounds(mp_obj_t self_in);
extern mp_obj_t ModPicoGraphics_set_framebuffer(mp_obj_t self_in, mp_obj_t framebuffer);

extern mp_int_t ModPicoGraphics_get_framebuffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);