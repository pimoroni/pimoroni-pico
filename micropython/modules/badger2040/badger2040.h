// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Badger2040_type;

/***** Extern of Class Methods *****/
extern void Badger2040_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Badger2040___del__(mp_obj_t self_in);
extern mp_obj_t Badger2040_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t Badger2040_update(mp_obj_t self_in);
extern mp_obj_t Badger2040_partial_update(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t Badger2040_led(mp_obj_t self_in, mp_obj_t brightness);
extern mp_obj_t Badger2040_pen(mp_obj_t self_in, mp_obj_t color);
extern mp_obj_t Badger2040_thickness(mp_obj_t self_in, mp_obj_t thickness);

extern mp_obj_t Badger2040_clear(mp_obj_t self_in);
extern mp_obj_t Badger2040_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y);
extern mp_obj_t Badger2040_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Badger2040_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);