// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Badger2040_type;

/***** Extern of Class Methods *****/
extern mp_obj_t Badger2040___del__(mp_obj_t self_in);
extern mp_obj_t Badger2040_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t Badger2040_is_busy(mp_obj_t self_in);
extern mp_obj_t Badger2040_update_speed(mp_obj_t self_in, mp_obj_t speed);
extern mp_obj_t Badger2040_update(mp_obj_t self_in);
extern mp_obj_t Badger2040_partial_update(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_halt(mp_obj_t self_in);

extern mp_obj_t Badger2040_invert(mp_obj_t self_in, mp_obj_t invert);
extern mp_obj_t Badger2040_led(mp_obj_t self_in, mp_obj_t brightness);
extern mp_obj_t Badger2040_font(mp_obj_t self_in, mp_obj_t font);
extern mp_obj_t Badger2040_pen(mp_obj_t self_in, mp_obj_t color);
extern mp_obj_t Badger2040_thickness(mp_obj_t self_in, mp_obj_t thickness);

extern mp_obj_t Badger2040_pressed(mp_obj_t self_in, mp_obj_t button);

extern mp_obj_t Badger2040_clear(mp_obj_t self_in);
extern mp_obj_t Badger2040_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t Badger2040_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Badger2040_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Badger2040_icon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Badger2040_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Badger2040_measure_glyph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_command(mp_obj_t self_in, mp_obj_t reg, mp_obj_t data);

extern mp_obj_t Badger2040_pressed_to_wake(mp_obj_t button);
extern mp_obj_t Badger2040_woken_by_button();

extern mp_obj_t Badger2040_system_speed(mp_obj_t speed);
